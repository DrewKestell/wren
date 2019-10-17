#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UILabel : public UIComponent
{
	const float width;
	ID2D1SolidColorBrush* textBrush{ nullptr };
	IDWriteTextFormat* textFormat{ nullptr };
	ComPtr<IDWriteTextLayout> textLayout;
	std::string text;
	
public:
	UILabel(UIComponentArgs uiComponentArgs, const float width);
	void Initialize(ID2D1SolidColorBrush* textBrush, IDWriteTextFormat* textFormat);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
    void SetText(const char* arr);
	void CreateTextLayout();
};