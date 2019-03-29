#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Publisher.h"

class UIButton : public UIComponent, public Observer, public Publisher
{
    const char* buttonText[20];
    bool pressed = false;
    bool enabled = true;
	const std::string& buttonId;
    float width;
    float height;
    ID2D1SolidColorBrush* buttonBrush;
    ID2D1SolidColorBrush* pressedButtonBrush;
    ID2D1SolidColorBrush* buttonBorderBrush;
    ID2D1SolidColorBrush* buttonTextBrush;
    ID2D1SolidColorBrush* disabledBrush;
    ID2D1RoundedRectangleGeometry* buttonGeometry;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* buttonTextLayout;
public:
    UIButton(
        const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
		EventHandler& eventHandler,
		const std::string& buttonId,
        const float width,
        const float height,
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
		Observer(eventHandler),
		Publisher(eventHandler),
		buttonId{ buttonId },
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
        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), &buttonGeometry);

    }
    bool IsEnabled();
    void SetEnabled(const bool isEnabled);
    virtual void Draw();
	virtual bool HandleEvent(const Event& event);
};
