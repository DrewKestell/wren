#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UICharacterListing : public UIComponent
{
	EventHandler& eventHandler;
	float width;
	float height;
	std::string characterName;
	ID2D1SolidColorBrush* brush{ nullptr };
	ID2D1SolidColorBrush* selectedBrush{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* textBrush{ nullptr };
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> geometry;
	bool selected{ false };
	
public:
	UICharacterListing(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		const float width,
		const float height,
		const char* text);
	void Initialize(
		ID2D1SolidColorBrush* brush,
		ID2D1SolidColorBrush* selectedBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormat
	);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string& GetName() const { return characterName; }
	const bool IsSelected() const { return selected; }
	void CreateGeometry();
};