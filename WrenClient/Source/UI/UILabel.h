#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UILabel : public UIComponent
{
	ComPtr<IDWriteTextLayout> textLayout;
	const float width{ 0.0f };
    char text[200];
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormat;
public:
	UILabel(
		UIComponentArgs uiComponentArgs,
		const float width,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormat);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
    void SetText(const char* arr);
};