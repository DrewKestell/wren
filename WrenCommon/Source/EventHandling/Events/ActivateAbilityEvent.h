#pragma once

#include "Event.h"

class ActivateAbilityEvent : public Event
{
public:
	ActivateAbilityEvent(const int abilityId)
		: Event(EventType::ActivateAbility),
		  abilityId{ abilityId }
	{
	}
	const int abilityId;
};