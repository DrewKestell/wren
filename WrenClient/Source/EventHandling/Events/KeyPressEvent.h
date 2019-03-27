#pragma once

#include "Event.h"

class KeyPressEvent : public Event
{
public:
	KeyPressEvent(const EventType eventType, const wchar_t charCode)
		: Event(eventType),
		charCode{ charCode }
	{
	}
	virtual ~KeyPressEvent() = 0;
	wchar_t charCode;
};