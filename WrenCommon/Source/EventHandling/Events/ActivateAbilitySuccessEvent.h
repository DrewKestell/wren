#pragma once

#include "Event.h"

class ActivateAbilitySuccessEvent : public Event
{
public:
	ActivateAbilitySuccessEvent(const int abilityId)
		: Event(EventType::ActivateAbilitySuccess),
		  abilityId{ abilityId }
	{
	}
	const int abilityId;
};