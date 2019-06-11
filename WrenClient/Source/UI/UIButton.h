#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIButton : public UIComponent
{
	ComPtr<IDWriteTextLayout> buttonTextLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> buttonGeometry;
	bool pressed{ false };
	bool enabled{ true };
	ID2D1SolidColorBrush* disabledBrush{ nullptr };
    float width;
	float height;
	const std::function<void()> onClick;
	ID2D1SolidColorBrush* buttonBrush;
    ID2D1SolidColorBrush* pressedButtonBrush;
    ID2D1SolidColorBrush* buttonBorderBrush;
    ID2D1SolidColorBrush* buttonTextBrush;
    ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;
    
public:
	UIButton(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const int zIndex,
		const float width,
		const float height,
		const char* inButtonText,
		const std::function<void()> onClick,
		ID2D1SolidColorBrush* buttonBrush,
		ID2D1SolidColorBrush* pressedButtonBrush,
		ID2D1SolidColorBrush* buttonBorderBrush,
		ID2D1SolidColorBrush* buttonTextBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* buttonTextFormat,
		ID2D1Factory2* d2dFactory);
    virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};
