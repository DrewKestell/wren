#pragma once

#include "Event.h"
#include <Windows.h>

class SystemKeyPressEvent : public Event
{
public:
	SystemKeyPressEvent(const EventType eventType, const WPARAM keyCode)
		: Event(eventType),
		  keyCode{ keyCode }
	{
	}
	virtual ~SystemKeyPressEvent() = 0;
	const WPARAM keyCode;
};