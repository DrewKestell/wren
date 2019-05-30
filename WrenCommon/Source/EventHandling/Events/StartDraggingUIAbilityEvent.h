#pragma once

#include "MouseEvent.h"

class StartDraggingUIAbilityEvent : public MouseEvent
{
public:
	StartDraggingUIAbilityEvent(const float mousePosX, const float mousePosY, const char hotbarIndex)
		: MouseEvent(EventType::StartDraggingUIAbility, mousePosX, mousePosY),
		  hotbarIndex{ hotbarIndex }
	{
	}
	const char hotbarIndex;
};