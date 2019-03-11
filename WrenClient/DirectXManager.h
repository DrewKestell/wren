#ifndef DIRECTXMANAGER_H
#define DIRECTXMANAGER_H

#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <string>
#include "GameTimer.h"

class DirectXManager
{
    UINT clientWidth;
    UINT clientHeight;
    IDWriteTextFormat* textFormatFPS = nullptr;
    IDWriteTextLayout* textLayoutFPS = nullptr;
    IDWriteTextLayout* textLayoutMousePos = nullptr;
    IDWriteTextFormat* textFormatAccountCreds = nullptr;
    IDWriteTextLayout* textLayoutAccountName = nullptr;
    IDWriteTextLayout* textLayoutPassword = nullptr;
    IDWriteTextLayout* textLayoutLoginButton = nullptr;
    IDWriteTextFormat* textFormatAccountCredsInputValue = nullptr;
    IDWriteTextLayout* textLayoutAccountNameInputValue = nullptr;
    IDWriteTextLayout* textLayoutPasswordInputValue = nullptr;
    GameTimer& timer;
    IDXGISwapChain* swapChain;
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
    ID2D1Factory2* d2dFactory;
    ID2D1DeviceContext1* d2dDeviceContext;
    ID3D11DepthStencilView* mDepthStencilView;
    ID2D1RoundedRectangleGeometry* roundedRectangleGeometry1;
    ID2D1RoundedRectangleGeometry* roundedRectangleGeometry2;
    ID2D1RoundedRectangleGeometry* loginButtonGeometry;
    IDWriteFactory2* writeFactory;
    ID2D1SolidColorBrush* grayBrush;
    ID2D1SolidColorBrush* blackBrush;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* blueBrush;
    ID2D1SolidColorBrush* darkBlueBrush;
public:
    DirectXManager(GameTimer& timer);
    void Initialize(HWND hWnd);
    void InitializeLoginScreen();
    void DrawScene(int mouseX, int mouseY, bool accountNameInputActive, bool passwordInputActive, bool loginButtonPressed, const char* accountNameInputValue, const char* passwordInputValue);
};

#endif