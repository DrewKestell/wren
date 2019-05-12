#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIHotbar : public UIComponent
{
	ComPtr<ID2D1RoundedRectangleGeometry> geometry[10];
	ID2D1SolidColorBrush* brush = nullptr;
	ID2D1DeviceContext1* d2dDeviceContext = nullptr;
	ID2D1Factory2* d2dFactory = nullptr;
public:
	UIHotbar(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		ID2D1SolidColorBrush* brush,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};