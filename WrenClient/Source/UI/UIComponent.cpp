#include "stdafx.h"
#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

extern EventHandler g_eventHandler;

UIComponent::UIComponent(std::vector<UIComponent*>& uiComponents, const XMFLOAT2 localPosition, const Layer uiLayer, const int zIndex)
	: uiComponents{ uiComponents },
	  localPosition{ localPosition },
	  uiLayer{ uiLayer },
	  zIndex{ zIndex }
{
	uiComponents.push_back(this);
	g_eventHandler.Subscribe(*this);
}

XMFLOAT2 UIComponent::GetWorldPosition() const
{
	auto worldPosition = localPosition;
	auto parentPtr = parent;
	while (parentPtr != nullptr)
	{
		worldPosition = Utility::XMFLOAT2Sum(worldPosition, parentPtr->localPosition);
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

void UIComponent::ClearChildren()
{
	for (auto i = 0; i < children.size(); i++)
	{
		if (children.at(i))
			delete children.at(i);
	}
	children.clear();
}