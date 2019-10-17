#include "stdafx.h"
#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

extern EventHandler g_eventHandler;

UIComponent::UIComponent(UIComponentArgs uiComponentArgs, const bool followParentVisibility)
	: deviceResources{ uiComponentArgs.deviceResources },
	  uiComponents{ uiComponentArgs.uiComponents },
	  calculatePosition{ uiComponentArgs.calculatePosition },
	  uiLayer{ uiComponentArgs.uiLayer },
	  zIndex{ uiComponentArgs.zIndex },
	  followParentVisibility{ followParentVisibility }
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

void UIComponent::ClearChildren()
{
	children.clear();
}