#ifndef UIINPUT_H
#define UIINPUT_H

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>

class UIInput
{
    char inputValue[32];
    bool active;
    int locationX;
    int locationY;
    int labelWidth;
    int inputWidth;
    int height;
    ID2D1SolidColorBrush* labelBrush;
    ID2D1SolidColorBrush* inputBrush;
    ID2D1SolidColorBrush* inputBorderBrush;
    ID2D1SolidColorBrush* inputValueBrush;
    ID2D1RoundedRectangleGeometry* inputGeometry;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* labelTextLayout;
public:
    UIInput(
        const int locationX,
        const int locationY,
        const int labelWidth,
        const int inputWidth,
        const int height,
        ID2D1SolidColorBrush* labelBrush,
        ID2D1SolidColorBrush* inputBrush,
        ID2D1SolidColorBrush* inputBorderBrush,
        ID2D1SolidColorBrush* inputValueBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inLabelText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* labelTextFormat,
        ID2D1Factory2* d2dFactory) :
        locationX{ locationX },
        locationY{ locationY },
        labelWidth{ labelWidth },
        inputWidth{ inputWidth },
        height{ height },
        labelBrush{ labelBrush },
        inputBrush{ inputBrush },
        inputBorderBrush{ inputBorderBrush },
        inputValueBrush{ inputValueBrush },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(inputValue, sizeof(inputValue));

        std::wostringstream outLabelText;
        outLabelText << inLabelText;
        if (FAILED(writeFactory->CreateTextLayout(outLabelText.str().c_str(), (UINT32)outLabelText.str().size(), labelTextFormat, labelWidth, height, &labelTextLayout)))
            throw std::exception("Failed to create text layout for UIInput.");

        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX + labelWidth + 10, locationY, locationX + labelWidth + inputWidth, locationY + height), 3.0f, 3.0f), &inputGeometry);
    }
    void Draw();
};

#endif