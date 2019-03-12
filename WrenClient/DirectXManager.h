#ifndef DIRECTXMANAGER_H
#define DIRECTXMANAGER_H

#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <string>
#include "GameTimer.h"
#include "LoginState.h"
#include "UIInput.h"
#include "UIButton.h"

class DirectXManager
{
    UINT clientWidth;
    UINT clientHeight;
    GameTimer& timer;

    // DXGI
    IDXGISwapChain* swapChain;

    // D3D
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;

    // D2D
    ID2D1Factory2* d2dFactory;
    ID2D1DeviceContext1* d2dDeviceContext;
    ID2D1RoundedRectangleGeometry* accountNameInputGeometry;
    ID2D1RoundedRectangleGeometry* passwordInputGeometry;
    ID2D1RoundedRectangleGeometry* loginButtonGeometry;

    // DirectWrite
    IDWriteFactory2* writeFactory;

    // TextFormats
    IDWriteTextFormat* textFormatFPS = nullptr;
    IDWriteTextFormat* textFormatAccountCreds = nullptr;
    IDWriteTextFormat* textFormatAccountCredsInputValue = nullptr;

    // TextLayouts
    IDWriteTextLayout* textLayoutFPS = nullptr;
    IDWriteTextLayout* textLayoutMousePos = nullptr;
    IDWriteTextLayout* textLayoutAccountName = nullptr;
    IDWriteTextLayout* textLayoutPassword = nullptr;
    IDWriteTextLayout* textLayoutLoginButton = nullptr;
    IDWriteTextLayout* textLayoutAccountNameInputValue = nullptr;
    IDWriteTextLayout* textLayoutPasswordInputValue = nullptr;

    // Brushes
    ID2D1SolidColorBrush* grayBrush;
    ID2D1SolidColorBrush* blackBrush;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* blueBrush;
    ID2D1SolidColorBrush* darkBlueBrush;

    // Inputs
    UIInput* accountNameInput;
    UIInput* passwordInput;
    UIInput* newAccountAccountNameInput;
    UIInput* newAccountPasswordInput;

    // Buttons
    UIButton* loginButton;
    UIButton* createAccountButton;

    void InitializeBrushes();
    void InitializeGeometry();
    void InitializeTextFormats();
    void InitializeInputs();
    void InitializeButtons();
public:
    DirectXManager(GameTimer& timer);
    void Initialize(HWND hWnd);
    void InitializeLoginScreen();
    void DrawScene(int mouseX, int mouseY, bool accountNameInputActive, bool passwordInputActive, bool loginButtonPressed, const char* accountNameInputValue, const char* passwordInputValue, LoginState loginState);
};

#endif