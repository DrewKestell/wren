#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIAbility : public UIComponent
{
	ID2D1DeviceContext1* d2dDeviceContext{ nullptr };
public:
	UIAbility(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};