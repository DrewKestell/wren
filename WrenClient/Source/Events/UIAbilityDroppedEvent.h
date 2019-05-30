#pragma once

#include <memory>
#include <EventHandling/Events/MouseEvent.h>
#include "../UI/UIAbility.h"

class UIAbilityDroppedEvent : public MouseEvent
{
public:
	UIAbilityDroppedEvent(UIAbility* uiAbility, const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::UIAbilityDroppedEvent, mousePosX, mousePosY),
		  uiAbility { uiAbility }
	{
	}
	UIAbility* uiAbility;
};