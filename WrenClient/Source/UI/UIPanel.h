#pragma once

#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/Event.h"

const FLOAT HEADER_HEIGHT = 20.0f;

class UIPanel : public UIComponent, public Observer
{
	ComPtr<ID2D1RoundedRectangleGeometry> headerGeometry;
	ComPtr<ID2D1RoundedRectangleGeometry> bodyGeometry;
	bool isActive{ false };
    bool isDragging{ false };
	float lastDragX{ 0.0f };
	float lastDragY{ 0.0f };
	const bool isDraggable{ false };
	float width{ 0.0f };
	float height{ 0.0f };
	WPARAM showKey{ 0 };
    ID2D1SolidColorBrush* headerBrush = nullptr;
    ID2D1SolidColorBrush* bodyBrush = nullptr;
    ID2D1SolidColorBrush* borderBrush = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    ID2D1Factory2* d2dFactory = nullptr;
public:
	UIPanel(
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		const bool isDraggable,
		const float width,
		const float height,
		const WPARAM showKey,
		ID2D1SolidColorBrush* headerBrush,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);
    virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};
