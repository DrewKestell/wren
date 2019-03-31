#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include <functional>
#include "UIComponent.h"
#include "../EventHandling/Observer.h"

class UIButton : public UIComponent, public Observer
{
    bool pressed = false;
    bool enabled = true;
    float width;
    float height;
	const std::function<void()> onClick;
	ID2D1SolidColorBrush* buttonBrush = nullptr;
    ID2D1SolidColorBrush* pressedButtonBrush = nullptr;
    ID2D1SolidColorBrush* buttonBorderBrush = nullptr;
    ID2D1SolidColorBrush* buttonTextBrush = nullptr;
    ID2D1SolidColorBrush* disabledBrush = nullptr;
    ID2D1RoundedRectangleGeometry* buttonGeometry = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
	ID2D1Factory2* d2dFactory = nullptr;
    IDWriteTextLayout* buttonTextLayout = nullptr;
public:
    UIButton(
        const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
        const float width,
        const float height,
		const std::function<void()> onClick,
        ID2D1SolidColorBrush* buttonBrush,
        ID2D1SolidColorBrush* pressedButtonBrush,
        ID2D1SolidColorBrush* buttonBorderBrush,
        ID2D1SolidColorBrush* buttonTextBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inButtonText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* buttonTextFormat,
        ID2D1Factory2* d2dFactory) :
        UIComponent(objectManager, position, uiLayer),
        width{ width },
        height{ height },
		onClick{ onClick },
        buttonBrush{ buttonBrush },
        pressedButtonBrush{ pressedButtonBrush },
        buttonBorderBrush{ buttonBorderBrush },
        buttonTextBrush{ buttonTextBrush },
        d2dDeviceContext{ d2dDeviceContext },
		d2dFactory{ d2dFactory }
    {
		std::wostringstream buttonText;
        buttonText << inButtonText;
        if (FAILED(writeFactory->CreateTextLayout(buttonText.str().c_str(), (UINT32)buttonText.str().size(), buttonTextFormat, width, height, &buttonTextLayout)))
            throw std::exception("Failed to create text layout for UIInput.");

    }
    virtual void Draw();
	virtual bool HandleEvent(const Event* event);
};
