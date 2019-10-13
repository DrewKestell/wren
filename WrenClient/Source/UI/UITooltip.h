#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

class UITooltip : public UIComponent
{
	EventHandler& eventHandler;
	ID2D1SolidColorBrush* bodyBrush;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormatTitle;
	IDWriteTextFormat* textFormatDescription;
	ComPtr<ID2D1RoundedRectangleGeometry> bodyGeometry;
	ComPtr<IDWriteTextLayout> headerTextLayout;
	ComPtr<IDWriteTextLayout> bodyTextLayout;
public:
	UITooltip(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		const std::string& headerText,
		const std::string& bodyText,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};
