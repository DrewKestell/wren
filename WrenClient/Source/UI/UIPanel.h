#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

static constexpr auto HEADER_HEIGHT = 20.0f;

class UIPanel : public UIComponent
{
	EventHandler& eventHandler;
	ComPtr<ID2D1RoundedRectangleGeometry> headerGeometry;
	ComPtr<ID2D1RoundedRectangleGeometry> bodyGeometry;
	bool isActive{ false };
    bool isDragging{ false };
	float lastDragX{ 0.0f };
	float lastDragY{ 0.0f };
	const bool isDraggable;
	float width;
	float height;
	WPARAM showKey;
	ID2D1SolidColorBrush* headerBrush;
    ID2D1SolidColorBrush* bodyBrush;
    ID2D1SolidColorBrush* borderBrush;
	void SetChildrenAsVisible(UIComponent* uiComponent);
	void BringToFront(UIComponent* uiComponent);
public:
	UIPanel(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		const bool isDraggable,
		const float width,
		const float height,
		const WPARAM showKey,
		ID2D1SolidColorBrush* headerBrush,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void ToggleVisibility();
	const bool GetIsDragging() const;
};
