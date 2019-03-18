#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>

class UILabel
{
    char text[200];
    FLOAT locationX;
    FLOAT locationY;
    FLOAT width;
    ID2D1SolidColorBrush* textBrush;
    IDWriteTextFormat* textFormat;
    IDWriteFactory2* writeFactory;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* textLayout;
public:
    UILabel(
        const FLOAT locationX,
        const FLOAT locationY,
        const FLOAT width,
        ID2D1SolidColorBrush* textBrush,
        IDWriteTextFormat* textFormat,
        ID2D1DeviceContext1* d2dDeviceContext,
        IDWriteFactory2* writeFactory,
        ID2D1Factory2* d2dFactory) :
        locationX{ locationX },
        locationY{ locationY },
        width{ width },
        textBrush{ textBrush },
        textFormat{ textFormat },
        writeFactory{ writeFactory },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(text, sizeof(text));
    }
    void Draw();
    void SetText(const char* arr);
};