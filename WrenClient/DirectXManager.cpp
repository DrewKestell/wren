#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
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

    //// Release reference to back-buffer
    //backBuffer->Release();

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
    IDWriteFactory2* writeFactory;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    if (FAILED(hr))
        throw std::exception("Failed to initialize DirectWrite Factory.");

    // Direct2DFactory
    ID2D1Factory2* d2dFactory;
    D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, _uuidof(ID2D1Factory), &options, reinterpret_cast<void**>(&d2dFactory));
    if (FAILED(hr))
        throw std::exception("Failed to create Direct2D Factory.");
    
    // Get the dxgi device
    IDXGIDevice1* dxgiDevice;
    hr = device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Device.");

    // Create D2D Device
    ID2D1Device1* d2dDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    if (FAILED(hr))
        throw std::exception("Failed to get Direct2D Device.");

    // Create D2D Device Context
    ID2D1DeviceContext1* d2dDeviceContext;
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
    if (FAILED(hr))
        throw std::exception("Failed to get Direct2D Device Context.");

    // Set D2D Render Target to back buffer
    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96.0f;
    bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = nullptr;

    // Get reference to DXGI back-buffer
    IDXGISurface* dxgiBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&dxgiBuffer));
    if (FAILED(hr))
        throw std::exception("Failed to retrieve the back buffer.");

    // create the bitmap
    ID2D1Bitmap1* targetBitmap;
    hr = d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer, &bp, &targetBitmap);
    if (FAILED(hr))
        throw std::exception("Failed to create the Direct2D bitmap from the DXGI surface.");

    // set the newly created bitmap as render target
    d2dDeviceContext->SetTarget(targetBitmap);

    // create standard brushes
    ID2D1SolidColorBrush* yellowBrush;
    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &yellowBrush)))
        throw std::exception("Critical error: Unable to create the yellow brush!");

    ID2D1SolidColorBrush* blackBrush;
    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackBrush)))
        throw std::exception("Critical error: Unable to create the black brush!");

    ID2D1SolidColorBrush* whiteBrush;
    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush)))
        throw std::exception("Critical error: Unable to create the white brush!");

    // set up text formats

    // FPS text
    IDWriteTextFormat* textFormatFPS;
    if (FAILED(writeFactory->CreateTextFormat(L"Lucida Console", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-GB", &textFormatFPS)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");
}

void DirectXManager::DrawScene()
{
    const float color[4] = { 1.0f, 0.5f, 0.3f, 1.0f };
    immediateContext->ClearRenderTargetView(renderTargetView, color);
    HRESULT hr = swapChain->Present(0, 0);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_SWAP_BUFFER);

    // draw text
}