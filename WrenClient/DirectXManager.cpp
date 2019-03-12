#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <iostream>
#include <sstream>
#include <string>
#include "DirectXManager.h"
#include <exception>
#include "Windows.h"
#include "LoginState.h"

constexpr auto FAILED_TO_CREATE_DEVICE = "Failed to create device.";
constexpr auto FAILED_TO_GET_BACK_BUFFER = "Failed to get pointer to back buffer.";
constexpr auto FAILED_TO_SWAP_BUFFER = "Failed to swap buffer.";

DirectXManager::DirectXManager(GameTimer& timer) : timer { timer } {};

void DirectXManager::Initialize(HWND hWnd)
{
    HRESULT hr;
    ID3D11Device* device;
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
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        featureLevelsRequested,
        numFeatureLevelsRequested,
        D3D11_SDK_VERSION,
        &device,
        &featureLevelsSupported,
        &immediateContext
    );
    if (FAILED(hr))
        throw std::exception(FAILED_TO_CREATE_DEVICE);

    UINT m4xMsaaQuality;
    hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m4xMsaaQuality);    if (FAILED(hr))
        throw std::exception("Failed to check MSAA quality.");

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 4;
    sd.SampleDesc.Quality = m4xMsaaQuality - 1;
    sd.Windowed = TRUE;
    sd.Flags = 0;

    RECT rect;
    GetClientRect(hWnd, &rect);
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;

    // Get the dxgi device
    IDXGIDevice* dxgiDevice;
    hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Device.");

    // Get the dxgi adapter
    IDXGIAdapter* dxgiAdapter;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Adapter.");

    // Get the dxgi factory interface
    IDXGIFactory* dxgiFactory;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Factory.");

    // Create swap chain
    hr = dxgiFactory->CreateSwapChain(device, &sd, &swapChain);
    if (FAILED(hr))
        throw std::exception("Failed to create swap chain.");

    // Get pointer to back buffer
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
        throw std::exception(FAILED_TO_GET_BACK_BUFFER);

    // Create a render-target view   
    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

    // Create Depth/Stencil Buffer and View
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 4;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* mDepthStencilBuffer;   
    hr = device->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);
    if (FAILED(hr))
        throw std::exception("Failed to create texture 2d.");
    hr = device->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);
    if (FAILED(hr))
        throw std::exception("Failed to create texture depth stencil view.");

    // Bind the view to output/merger stage
    immediateContext->OMSetRenderTargets(1, &renderTargetView, mDepthStencilView);

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
    D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, _uuidof(ID2D1Factory), &options, reinterpret_cast<void**>(&d2dFactory));
    if (FAILED(hr))
        throw std::exception("Failed to create Direct2D Factory.");

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

    InitializeBrushes();
    InitializeTextFormats();
    InitializeInputs();
    InitializeButtons();
}

void DirectXManager::InitializeBrushes()
{
    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), &grayBrush)))
        throw std::exception("Critical error: Unable to create the gray brush!");

    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.22f, 0.22f, 1.0f), &blackBrush)))
        throw std::exception("Critical error: Unable to create the black brush!");

    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &whiteBrush)))
        throw std::exception("Critical error: Unable to create the white brush!");

    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 1.0f), &blueBrush)))
        throw std::exception("Critical error: Unable to create the blue brush!");

    if (FAILED(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.301f, 0.729f, 1.0f, 1.0f), &darkBlueBrush)))
        throw std::exception("Critical error: Unable to create the darkBlue brush!");
}

void DirectXManager::InitializeTextFormats()
{
    const WCHAR* arialFontFamily = L"Arial";
    const WCHAR* locale = L"en-US";

    // FPS / MousePos
    if (FAILED(writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, &textFormatFPS)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");

    // Account Creds Input Values
    if (FAILED(writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormatAccountCredsInputValue)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");

    // Account Creds Labels
    if (FAILED(writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormatAccountCreds)))
        throw std::exception("Critical error: Unable to create text format for FPS information!");
    if (FAILED(textFormatAccountCreds->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)))
        throw std::exception("Critical error: Unable to set text alignment!");
    if (FAILED(textFormatAccountCreds->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)))
        throw std::exception("Critical error: Unable to set paragraph alignment!");
}

void DirectXManager::InitializeInputs()
{
    accountNameInput = new UIInput(15, 20, 120, 260, 24, blackBrush, whiteBrush, grayBrush, blackBrush, d2dDeviceContext, "Account Name:", writeFactory, textFormatAccountCreds, d2dFactory);
    passwordInput = new UIInput(15, 50, 120, 260, 24, blackBrush, whiteBrush, grayBrush, blackBrush, d2dDeviceContext, "Password:", writeFactory, textFormatAccountCreds, d2dFactory);
}

void DirectXManager::InitializeButtons()
{
    loginButton = new UIButton(15, 100, 80, 24, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "LOGIN", writeFactory, textFormatFPS, d2dFactory);
}

void DirectXManager::DrawScene(int mouseX, int mouseY, bool accountNameInputActive, bool passwordInputActive, bool loginButtonPressed, const char* accountNameInputValue, const char* passwordInputValue, LoginState loginState)
{
    const float color[4] = { 1.0f, 0.5f, 0.3f, 1.0f };
    immediateContext->ClearRenderTargetView(renderTargetView, color);
    immediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    d2dDeviceContext->BeginDraw();

    // --- LOGIN SCREEN ---
    accountNameInput->Draw();
    passwordInput->Draw();
    loginButton->Draw();

    // draw input text
    /*std::wostringstream outAccountNameInputValue;
    outAccountNameInputValue << accountNameInputValue;
    if (accountNameInputActive)
        outAccountNameInputValue << "|";
    if (FAILED(writeFactory->CreateTextLayout(outAccountNameInputValue.str().c_str(), (UINT32)outAccountNameInputValue.str().size(), textFormatAccountCredsInputValue, 260, 100, &textLayoutAccountNameInputValue)))
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(146.0f, 18.5f), textLayoutAccountNameInputValue, blackBrush);

    std::wostringstream outPasswordInputValue;

    auto i = 0;
    std::string hiddenPassword = "";
    while (i < strlen(passwordInputValue))
    {
        hiddenPassword += "*";
        i++;
    }
    outPasswordInputValue << hiddenPassword.c_str();
    if (passwordInputActive)
        outPasswordInputValue << "|";
    if (FAILED(writeFactory->CreateTextLayout(outPasswordInputValue.str().c_str(), (UINT32)outPasswordInputValue.str().size(), textFormatAccountCredsInputValue, 260, 100, &textLayoutPasswordInputValue)))
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(146.0f, 49.0f), textLayoutPasswordInputValue, blackBrush);*/

    // --- CREATE ACCOUNT SCREEN ---

    // --- SHARED ---
    // draw FPS
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;
    frameCnt++;

    if (timer.TotalTime() - timeElapsed >= 1)
    {
        float fps = (float)frameCnt;

        std::wostringstream outFPS;
        outFPS.precision(6);
        outFPS << "FPS: " << fps;

        if (FAILED(writeFactory->CreateTextLayout(outFPS.str().c_str(), (UINT32)outFPS.str().size(), textFormatFPS, (float)clientWidth, (float)clientHeight, &textLayoutFPS)))
            throw std::exception("Critical error: Failed to create the text layout for FPS information!");

        frameCnt = 0;
        timeElapsed += 1.0f;
    }

    if (textLayoutFPS != nullptr)
        d2dDeviceContext->DrawTextLayout(D2D1::Point2F(540.0f, 540.0f), textLayoutFPS, blackBrush);

    // draw MousePos
    std::wostringstream outMousePos;
    outMousePos.precision(6);
    outMousePos << "MousePosX: " << mouseX << ", MousePosY: " << mouseY;
    writeFactory->CreateTextLayout(outMousePos.str().c_str(), (UINT32)outMousePos.str().size(), textFormatFPS, (float)clientWidth, (float)clientHeight, &textLayoutMousePos);
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(540.0f, 520.0f), textLayoutMousePos, blackBrush);

    if (FAILED(d2dDeviceContext->EndDraw()))
        throw std::exception("Failed to call EndDraw.");

    if (FAILED(swapChain->Present(0, 0)))
        throw std::exception(FAILED_TO_SWAP_BUFFER);
}
