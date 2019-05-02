#include "stdafx.h"
#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

extern EventHandler g_eventHandler;

UIComponent::UIComponent(std::vector<UIComponent*>& uiComponents, const XMFLOAT3 localPosition, const XMFLOAT3 scale, const Layer uiLayer)
	: uiComponents{ uiComponents },
	  localPosition{ localPosition },
	  scale{ scale },
	  uiLayer{ uiLayer }
{
	uiComponents.push_back(this);
	g_eventHandler.Subscribe(*this);
}

XMFLOAT3 UIComponent::GetWorldPosition() const
{
	auto worldPosition = localPosition;
	auto parentPtr = parent;
	while (parentPtr != nullptr)
	{
		worldPosition = Utility::XMFLOAT3Sum(worldPosition, parentPtr->localPosition);
		parentPtr = parentPtr->parent;
	}

	return worldPosition;
}

void UIComponent::Draw() {}

UIComponent::~UIComponent()
{
	uiComponents.erase(std::find(uiComponents.begin(), uiComponents.end(), this));
	g_eventHandler.Unsubscribe(*this);
}