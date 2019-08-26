#pragma once

#include <EventHandling/Events/Event.h>

class AttackMissEvent : public Event
{
public:
	AttackMissEvent(const unsigned int attackerId, const unsigned int targetId, const int weaponSkillIds[], const int weaponSkillArrLen)
		: Event(EventType::AttackMiss),
		  attackerId{ attackerId },
		  targetId{ targetId },
		  weaponSkillIds{ weaponSkillIds },
		  weaponSkillArrLen{ weaponSkillArrLen }
	{
	}
	const unsigned int attackerId;
	const unsigned int targetId;
	const int* weaponSkillIds;
	const int weaponSkillArrLen;
};