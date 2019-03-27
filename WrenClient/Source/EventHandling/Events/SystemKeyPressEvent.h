#pragma once

#include "Event.h"
#include "SystemKey.h"

class SystemKeyPressEvent : public Event
{
public:
	SystemKeyPressEvent(const EventType eventType, const SystemKey code)
		: Event(eventType),
		code{ code }
	{
	}
	virtual ~SystemKeyPressEvent() = 0;
	const SystemKey code;
};