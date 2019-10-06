#include "stdafx.h"
#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

extern EventHandler g_eventHandler;

UIComponent::UIComponent(std::vector<UIComponent*>& uiComponents, const XMFLOAT2 localPosition, const Layer uiLayer, const unsigned int zIndex)
	: uiComponents{ uiComponents },
	  localPosition{ localPosition },
	  uiLayer{ uiLayer },
	  zIndex{ zIndex }
{
	uiComponents.push_back(this);
}

XMFLOAT2 UIComponent::GetWorldPosition() const
{
	auto worldPosition = localPosition;
	auto parentPtr = parent;
	while (parentPtr != nullptr)
	{
		worldPosition = worldPosition + parentPtr->localPosition;
		parentPtr = parentPtr->parent;
	}

	return worldPosition;
}

void UIComponent::Draw() {}

UIComponent::~UIComponent()
{
	uiComponents.erase(std::find(uiComponents.begin(), uiComponents.end(), this));
}

void UIComponent::EmptyChildren()
{
	children.clear();
}

// TODO: this is broken because we have unique_ptrs and this blows up when we try to delete them
void UIComponent::ClearChildren()
{
	for (auto i = 0; i < children.size(); i++)
	{
		if (children.at(i))
			delete children.at(i);
	}
	children.clear();
}