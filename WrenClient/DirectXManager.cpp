#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include "DirectXManager.h"
#include <exception>
#include "Windows.h"

constexpr auto FAILED_TO_CREATE_DEVICE_AND_SWAP_CHAIN = "Failed to create device and swap chain.";
constexpr auto FAILED_TO_GET_BACK_BUFFER = "Failed to get pointer to back buffer.";
constexpr auto FAILED_TO_SWAP_BUFFER = "Failed to swap buffer.";

void DirectXManager::Initialize(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    UINT numFeatureLevelsRequested = 1;
    D3D_FEATURE_LEVEL featureLevelsSupported;

    HRESULT hr;
    // Create device, context and swap chain    
    ID3D11Device* device;
    hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        &featureLevelsRequested,
        numFeatureLevelsRequested,
        D3D11_SDK_VERSION,
        &sd,
        &swapChain,
        &device,
        &featureLevelsSupported,
        &immediateContext);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_CREATE_DEVICE_AND_SWAP_CHAIN);

    // Get pointer to back buffer
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_GET_BACK_BUFFER);

    // Create a render-target view   
    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

    // Release reference to back-buffer
    backBuffer->Release();

    // Bind the view to output/merger stage
    immediateContext->OMSetRenderTargets(1, &renderTargetView, NULL);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    immediateContext->RSSetViewports(1, &vp);

    // DWriteFactory
    IDWriteFactory* writeFactory;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);
    if (FAILED(hr))
        throw std::exception("Failed to initialize DirectWrite Factory.");
}

void DirectXManager::DrawScene()
{
    const float color[4] = { 1.0f, 0.5f, 0.3f, 1.0f };
    immediateContext->ClearRenderTargetView(renderTargetView, color);
    HRESULT hr = swapChain->Present(0, 0);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_SWAP_BUFFER);
}