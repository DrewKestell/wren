#pragma once

#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/Event.h"
#include "../ObjectManager.h"

class UIInput : public UIComponent, public Observer
{
    int inputIndex;
    wchar_t inputValue[30];
    bool active;
    bool secure;
    float labelWidth;
    float inputWidth;
    float height;
    ID2D1SolidColorBrush* labelBrush = nullptr;
    ID2D1SolidColorBrush* inputBrush = nullptr;
    ID2D1SolidColorBrush* inputBorderBrush = nullptr;
    ID2D1SolidColorBrush* inputValueBrush = nullptr;
    IDWriteTextFormat* inputValueTextFormat = nullptr;
    IDWriteFactory2* writeFactory = nullptr;
    ID2D1RoundedRectangleGeometry* inputGeometry = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    IDWriteTextLayout* labelTextLayout = nullptr;
    IDWriteTextLayout* inputValueTextLayout = nullptr;
public:
    UIInput(
        const XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
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
        UIComponent(objectManager, position, uiLayer),
		Observer(),
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
	virtual bool HandleEvent(const Event* event);
    const wchar_t* GetInputValue();
	bool IsActive() { return active; }
	void SetActive(bool active) { this->active = active; }
};
