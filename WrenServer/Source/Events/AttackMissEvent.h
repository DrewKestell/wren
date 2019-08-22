#pragma once

#include <EventHandling/Events/Event.h>

class AttackMissEvent : public Event
{
public:
	AttackMissEvent(const int attackerId, const int targetId, const int weaponSkillIds[], const int weaponSkillArrLen)
		: Event(EventType::AttackMiss),
		  attackerId{ attackerId },
		  targetId{ targetId },
		  weaponSkillIds{ weaponSkillIds },
		  weaponSkillArrLen{ weaponSkillArrLen }
	{
	}
	const int attackerId;
	const int targetId;
	const int* weaponSkillIds;
	const int weaponSkillArrLen;
};