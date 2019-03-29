#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include "UIComponent.h"

class UILabel : public UIComponent
{
    char text[200];
    float width;
    ID2D1SolidColorBrush* textBrush;
    IDWriteTextFormat* textFormat;
    IDWriteFactory2* writeFactory;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* textLayout;
public:
    UILabel(
        const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
        const float width,
        ID2D1SolidColorBrush* textBrush,
        IDWriteTextFormat* textFormat,
        ID2D1DeviceContext1* d2dDeviceContext,
        IDWriteFactory2* writeFactory,
        ID2D1Factory2* d2dFactory) :
        UIComponent(objectManager, position, uiLayer),
        width{ width },
        textBrush{ textBrush },
        textFormat{ textFormat },
        writeFactory{ writeFactory },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(text, sizeof(text));
    }
    virtual void Draw();
    void SetText(const char* arr);
};