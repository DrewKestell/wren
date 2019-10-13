#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UICharacterListing : public UIComponent
{
	EventHandler& eventHandler;
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> geometry;
	std::string characterName{ "" };
	bool selected{ false };
	float width{ 0.0f };
	float height{ 0.0f };
	ID2D1SolidColorBrush* brush;
    ID2D1SolidColorBrush* selectedBrush;
    ID2D1SolidColorBrush* borderBrush;
    ID2D1SolidColorBrush* textBrush;
    
public:
	UICharacterListing(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		const float width,
		const float height,
		const char* inText,
		ID2D1SolidColorBrush* brush,
		ID2D1SolidColorBrush* selectedBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormat);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string& GetName() const { return characterName; }
	const bool IsSelected() const { return selected; }
};