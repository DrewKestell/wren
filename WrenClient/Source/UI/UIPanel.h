#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

constexpr float HEADER_HEIGHT = 20.0f;

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
    ID2D1DeviceContext1* d2dDeviceContext;
    ID2D1Factory2* d2dFactory;
	void SetChildrenAsVisible(UIComponent* uiComponent);
	void BringToFront(UIComponent* uiComponent);
public:
	UIPanel(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		const bool isDraggable,
		const float width,
		const float height,
		const WPARAM showKey,
		ID2D1SolidColorBrush* headerBrush,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};
