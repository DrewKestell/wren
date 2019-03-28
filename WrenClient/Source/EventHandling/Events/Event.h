#pragma once

#include "EventType.h"
#include "../../Layer.h"

class Event
{
public:
	Event(const EventType type)
		: type { type }
	{
	}
	const EventType type;
};