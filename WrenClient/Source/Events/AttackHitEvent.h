#pragma once

#include <EventHandling/Events/Event.h>

class AttackHitEvent : public Event
{
public:
	AttackHitEvent(const int attackerId, const int targetId, const int damage)
		: Event(EventType::AttackHit),
		  attackerId{ attackerId },
		  targetId{ targetId },
		  damage{ damage }
	{
	}
	const int attackerId;
	const int targetId;
	const int damage;
};