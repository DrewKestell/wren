#pragma once

#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/Event.h"

class UIButton : public UIComponent, public Observer
{
	ComPtr<IDWriteTextLayout> buttonTextLayout ;
	ComPtr<ID2D1RoundedRectangleGeometry> buttonGeometry;
	bool pressed{ false };
	bool enabled{ true };
    float width{ 0.0f };
	float height{ 0.0f };
	const std::function<void()> onClick{};
	ID2D1SolidColorBrush* buttonBrush = nullptr;
    ID2D1SolidColorBrush* pressedButtonBrush = nullptr;
    ID2D1SolidColorBrush* buttonBorderBrush = nullptr;
    ID2D1SolidColorBrush* buttonTextBrush = nullptr;
    ID2D1SolidColorBrush* disabledBrush = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
	ID2D1Factory2* d2dFactory = nullptr;
    
public:
	UIButton(
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
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
