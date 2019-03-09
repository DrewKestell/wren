#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <iostream>
#include <sstream>
#include "DirectXManager.h"
#include <exception>
#include "Windows.h"

constexpr auto FAILED_TO_CREATE_DEVICE_AND_SWAP_CHAIN = "Failed to create device and swap chain.";
constexpr auto FAILED_TO_GET_BACK_BUFFER = "Failed to get pointer to back buffer.";
constexpr auto FAILED_TO_SWAP_BUFFER = "Failed to swap buffer.";

DirectXManager::DirectXManager(GameTimer& timer) : timer { timer } {};

void DirectXManager::Initialize(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 4;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsRequested[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
    UINT numFeatureLevelsRequested = 1;
    D3D_FEATURE_LEVEL featureLevelsSupported;

    HRESULT hr;
    // Create device, context and swap chain    
    ID3D11Device* device;
    hr = D3D11CreateDeviceAndSwapChain(
		NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevelsRequested,
        numFeatureLevelsRequested,
        D3D11_SDK_VERSION,
        &sd,
        &swapChain,
        &device,
        &featureLevelsSupported,
        &immediateContext);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_CREATE_DEVICE_AND_SWAP_CHAIN);

    UINT levels;
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &levels);

    // Get pointer to back buffer
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_GET_BACK_BUFFER);

    // Create a render-target view   
    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

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
    IDXGIDevice* dxgiDevice;
    hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Device.");

    // Create D2D Device
    ID2D1Device1* d2dDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    if (FAILED(hr))
        throw std::exception("Failed to get Direct2D Device.");

    // Create D2D Device Context
    
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dDeviceContext);
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
    IDXGISurface1* dxgiBuffer;
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
    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateGray), &blackBrush)))
        throw std::exception("Critical error: Unable to create the black brush!");

    // set up text formats
    if (FAILED(writeFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormatFPS)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");
}

void DirectXManager::InitializeLoginScreen()
{
    // set up text formats
    if (FAILED(writeFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", &textFormatAccountCreds)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatAccountCreds->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatAccountCreds->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");

    std::wostringstream outAccountName;
    outAccountName << "Account Name:";
    if (FAILED(writeFactory->CreateTextLayout(outAccountName.str().c_str(), (UINT32)outAccountName.str().size(), textFormatAccountCreds, (float)800, (float)600, &textLayoutAccountName)))
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");

    std::wostringstream outPassword;
    outPassword << "Password:";
    if (FAILED(writeFactory->CreateTextLayout(outPassword.str().c_str(), (UINT32)outPassword.str().size(), textFormatAccountCreds, (float)800, (float)600, &textLayoutPassword)))
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");
}

void DirectXManager::DrawScene()
{
    HRESULT hr;

    const float color[4] = { 1.0f, 0.5f, 0.3f, 1.0f };
    immediateContext->ClearRenderTargetView(renderTargetView, color);

    static int frameCnt = 0;
    static float timeElapsed = 0.0f;
    frameCnt++;

    if (timer.TotalTime() - timeElapsed >= 1)
    {
        float fps = (float)frameCnt; // fps = frameCnt / 1
        
        std::wostringstream outFPS;
        outFPS.precision(6);
        outFPS << "FPS: " << fps;

        hr = writeFactory->CreateTextLayout(outFPS.str().c_str(), (UINT32)outFPS.str().size(), textFormatFPS, (float)800, (float)600, &textLayoutFPS);
        if (FAILED(hr))
            throw std::exception("Critical error: Failed to create the text layout for FPS information!");

        frameCnt = 0;
        timeElapsed += 1.0f;
    }

    d2dDeviceContext->BeginDraw();

    // draw account creds
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(2.0f, 5.0f), textLayoutAccountName, blackBrush);
    //d2dDeviceContext->DrawTextLayout(D2D1::Point2F(static_cast<FLOAT>(4.0f / 96.0f), static_cast<FLOAT>(20.0f / 52.0f)), textLayoutPassword, blackBrush);

    // draw FPS
    if (textLayoutFPS != nullptr)
        d2dDeviceContext->DrawTextLayout(D2D1::Point2F(700.0f, 574.0f), textLayoutFPS, blackBrush);

    hr = d2dDeviceContext->EndDraw();

    hr = swapChain->Present(0, 0);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_SWAP_BUFFER);
}
