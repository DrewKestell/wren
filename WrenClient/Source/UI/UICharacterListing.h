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
    ID2D1DeviceContext1* d2dDeviceContext;
    
public:
	UICharacterListing(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		const float width,
		const float height,
		const char* inText,
		ID2D1SolidColorBrush* brush,
		ID2D1SolidColorBrush* selectedBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* textFormat,
		ID2D1Factory2* d2dFactory);
    virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	const std::string& GetName() const { return characterName; }
	const bool IsSelected() const { return selected; }
};