#ifndef DIRECTXMANAGER_H
#define DIRECTXMANAGER_H

#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include "GameTimer.h"

class DirectXManager
{
    IDWriteTextFormat* textFormatFPS = nullptr;
    IDWriteTextLayout* textLayoutFPS = nullptr;
    IDWriteTextFormat* textFormatAccountCreds = nullptr;
    IDWriteTextLayout* textLayoutAccountName = nullptr;
    IDWriteTextLayout* textLayoutPassword = nullptr;
    GameTimer& timer;
    IDXGISwapChain* swapChain;
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteFactory2* writeFactory;
    ID2D1SolidColorBrush* blackBrush;
public:
    DirectXManager(GameTimer& timer);
    void Initialize(HWND hWnd);
    void InitializeLoginScreen();
    void DrawScene();
};

#endif