#pragma once

#include <EventHandling/Events/MouseEvent.h>
#include "../UI/UIAbility.h"

class UIAbilityDroppedEvent : public MouseEvent
{
public:
	UIAbilityDroppedEvent(UIAbility* uiAbility, const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::UIAbilityDropped, mousePosX, mousePosY),
		  uiAbility { uiAbility }
	{
	}
	UIAbility* uiAbility;
};