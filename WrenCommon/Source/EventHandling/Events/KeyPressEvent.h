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
	const wchar_t charCode;
};