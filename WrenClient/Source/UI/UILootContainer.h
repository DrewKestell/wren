#pragma once

#include "UIComponent.h"
#include <EventHandling/EventHandler.h>

class UILootContainer : public UIComponent
{
	ComPtr<ID2D1RectangleGeometry> geometry[12];
	ID2D1SolidColorBrush* brush;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;

	void ReinitializeGeometry();
public:
	UILootContainer(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		ID2D1SolidColorBrush* brush,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};