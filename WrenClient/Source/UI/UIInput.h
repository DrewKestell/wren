#ifndef UIINPUT_H
#define UIINPUT_H

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include "../GameObject.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Publisher.h"

class UIInput : public GameObject, public Observer, public Publisher
{
    int inputIndex;
    wchar_t inputValue[30];
    bool active;
    bool secure;
    float labelWidth;
    float inputWidth;
    float height;
    ID2D1SolidColorBrush* labelBrush;
    ID2D1SolidColorBrush* inputBrush;
    ID2D1SolidColorBrush* inputBorderBrush;
    ID2D1SolidColorBrush* inputValueBrush;
    IDWriteTextFormat* inputValueTextFormat;
    IDWriteFactory2* writeFactory;
    ID2D1RoundedRectangleGeometry* inputGeometry;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* labelTextLayout;
    IDWriteTextLayout* inputValueTextLayout;
public:
    UIInput(
        const DirectX::XMFLOAT3 position,
		EventHandler* eventHandler,
        const bool secure,
        const float labelWidth,
        const float inputWidth,
        const float height,
        ID2D1SolidColorBrush* labelBrush,
        ID2D1SolidColorBrush* inputBrush,
        ID2D1SolidColorBrush* inputBorderBrush,
        ID2D1SolidColorBrush* inputValueBrush,
        IDWriteTextFormat* inputValueTextFormat,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inLabelText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* labelTextFormat,
        ID2D1Factory2* d2dFactory) :
        GameObject(position),
		Observer(eventHandler),
		Publisher(eventHandler),
        secure{ secure },
        labelWidth{ labelWidth },
        inputWidth{ inputWidth },
        height{ height },
        labelBrush{ labelBrush },
        inputBrush{ inputBrush },
        inputBorderBrush{ inputBorderBrush },
        inputValueBrush{ inputValueBrush },
        inputValueTextFormat{ inputValueTextFormat },
        writeFactory{ writeFactory },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(inputValue, sizeof(inputValue));

        std::wostringstream outLabelText;
        outLabelText << inLabelText;
        if (FAILED(writeFactory->CreateTextLayout(outLabelText.str().c_str(), (UINT32)outLabelText.str().size(), labelTextFormat, labelWidth, height, &labelTextLayout)))
            throw std::exception("Failed to create text layout for UIInput.");

        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x + labelWidth + 10, position.y, position.x + labelWidth + inputWidth, position.y + height), 3.0f, 3.0f), &inputGeometry);
    }
    virtual void Draw();
	virtual void HandleEvent(const Event& event);
    const wchar_t* GetInputValue();
    void Clear();
};

#endif