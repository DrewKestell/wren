#pragma once

#include <EventHandling/Events/Event.h>

class SkillIncreaseEvent : public Event
{
public:
	SkillIncreaseEvent(const int skillId, const int newValue)
		: Event(EventType::SkillIncrease),
		  skillId{ skillId },
		  newValue{ newValue }
	{
	}
	const int skillId;
	const int newValue;
};