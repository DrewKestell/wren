#pragma once

#include <EventHandling/Events/Event.h>

class AttackHitEvent : public Event
{
public:
	AttackHitEvent(const unsigned int attackerId, const unsigned int targetId, const int damage)
		: Event(EventType::AttackHit),
		  attackerId{ attackerId },
		  targetId{ targetId },
		  damage{ damage }
	{
	}
	const unsigned int attackerId;
	const unsigned int targetId;
	const int damage;
};