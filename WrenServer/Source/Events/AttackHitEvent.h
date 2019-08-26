#pragma once

#include <EventHandling/Events/Event.h>

class AttackHitEvent : public Event
{
public:
	AttackHitEvent(const unsigned int attackerId, const unsigned int targetId, const int damage, const int weaponSkillIds[], const int weaponSkillArrLen)
		: Event(EventType::AttackHit),
		  attackerId{ attackerId },
		  targetId{ targetId },
		  damage{ damage },
		  weaponSkillIds{ weaponSkillIds },
		  weaponSkillArrLen{ weaponSkillArrLen }
	{
	}
	const unsigned int attackerId;
	const unsigned int targetId;
	const int damage;
	const int* weaponSkillIds;
	const int weaponSkillArrLen;

	~AttackHitEvent() { delete[] weaponSkillIds; }

};