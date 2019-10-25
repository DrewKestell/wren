#include "stdafx.h"
#include "UIComponent.h"

extern float g_clientWidth;
extern float g_clientHeight;

UIComponent::UIComponent(UIComponentArgs uiComponentArgs, const bool followParentVisibility, const bool followParentZIndex)
	: deviceResources{ uiComponentArgs.deviceResources },
	  uiComponents{ uiComponentArgs.uiComponents },
	  calculatePosition{ uiComponentArgs.calculatePosition },
	  uiLayer{ uiComponentArgs.uiLayer },
	  zIndex{ uiComponentArgs.zIndex },
	  followParentVisibility{ followParentVisibility },
	  followParentZIndex{ followParentZIndex }
{
	uiComponents.push_back(this);
	localPosition = uiComponentArgs.calculatePosition(g_clientWidth, g_clientHeight);
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

const bool UIComponent::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::WindowResize:
		{
			localPosition = calculatePosition(g_clientWidth, g_clientHeight);

			break;
		}
	}

	return false;
}