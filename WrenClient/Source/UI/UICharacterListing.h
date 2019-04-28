#pragma once

#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/Event.h"

class UICharacterListing : public UIComponent, public Observer
{
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> geometry;
	std::string characterName{ "" };
	bool selected{ false };
	float width{ 0.0f };
	float height{ 0.0f };
    ID2D1SolidColorBrush* brush = nullptr;
    ID2D1SolidColorBrush* selectedBrush = nullptr;
    ID2D1SolidColorBrush* borderBrush = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    
public:
	UICharacterListing(
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
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