#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIButton : public UIComponent
{
	float width;
	float height;
	std::string buttonText;
	const std::function<void()> onClick;
	ID2D1SolidColorBrush* disabledBrush{ nullptr };
	ID2D1SolidColorBrush* buttonBrush{ nullptr };
	ID2D1SolidColorBrush* pressedButtonBrush{ nullptr };
	ID2D1SolidColorBrush* buttonBorderBrush{ nullptr };
	ID2D1SolidColorBrush* buttonTextBrush{ nullptr };
	ComPtr<IDWriteTextLayout> buttonTextLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> buttonGeometry;
	bool pressed{ false };
	bool enabled{ true };
    
public:
	UIButton(
		UIComponentArgs uiComponentArgs,
		const float width,
		const float height,
		const char* buttonText,
		const std::function<void()> onClick);
	void Initialize(
		ID2D1SolidColorBrush* buttonBrush,
		ID2D1SolidColorBrush* pressedButtonBrush,
		ID2D1SolidColorBrush* buttonBorderBrush,
		ID2D1SolidColorBrush* buttonTextBrush,
		IDWriteTextFormat* buttonTextFormat);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};
