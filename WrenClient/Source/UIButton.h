#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>

class UIButton
{
    const char* buttonText[20];
    bool pressed;
    int locationX;
    int locationY;
    int width;
    int height;
    ID2D1SolidColorBrush* buttonBrush;
    ID2D1SolidColorBrush* pressedButtonBrush;
    ID2D1SolidColorBrush* buttonBorderBrush;
    ID2D1SolidColorBrush* buttonTextBrush;
    ID2D1RoundedRectangleGeometry* buttonGeometry;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* buttonTextLayout;
public:
    UIButton(
        const int locationX,
        const int locationY,
        const int width,
        const int height,
        ID2D1SolidColorBrush* buttonBrush,
        ID2D1SolidColorBrush* pressedButtonBrush,
        ID2D1SolidColorBrush* buttonBorderBrush,
        ID2D1SolidColorBrush* buttonTextBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inButtonText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* buttonTextFormat,
        ID2D1Factory2* d2dFactory) :
        locationX{ locationX },
        locationY{ locationY },
        width{ width },
        height{ height },
        buttonBrush{ buttonBrush },
        pressedButtonBrush{ pressedButtonBrush },
        buttonBorderBrush{ buttonBorderBrush },
        buttonTextBrush{ buttonTextBrush },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(buttonText, sizeof(buttonText));

        std::wostringstream outButtonText;
        outButtonText << inButtonText;
        if (FAILED(writeFactory->CreateTextLayout(outButtonText.str().c_str(), (UINT32)outButtonText.str().size(), buttonTextFormat, width, height, &buttonTextLayout)))
            throw std::exception("Failed to create text layout for UIInput.");

        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, locationY, locationX + width, locationY + height), 3.0f, 3.0f), &buttonGeometry);

    }
    bool IsPressed();
    void SetPressed(bool isPressed);
    void Draw();
    bool DetectClick(int x, int y);
};

#endif