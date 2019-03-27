#pragma once

#include "Event.h"

class KeyPressEvent : public Event
{
public:
	KeyPressEvent(const EventType eventType, const int c)
		: Event(eventType),
		charCode{ charCode }
	{
	}
	virtual ~KeyPressEvent() = 0;
	const int charCode;
};