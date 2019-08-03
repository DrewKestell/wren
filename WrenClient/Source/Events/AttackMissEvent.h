#pragma once

#include <EventHandling/Events/Event.h>

class AttackMissEvent : public Event
{
public:
	AttackMissEvent(const int attackerId, const int targetId)
		: Event(EventType::AttackMiss),
		  attackerId{ attackerId },
		  targetId{ targetId }
	{
	}
	const int attackerId;
	const int targetId;
};