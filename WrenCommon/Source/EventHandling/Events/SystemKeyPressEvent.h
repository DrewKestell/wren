#pragma once

#include "Event.h"

class SystemKeyPressEvent : public Event
{
public:
	SystemKeyPressEvent(const EventType eventType, const WPARAM keyCode)
		: Event(eventType),
		  keyCode{ keyCode }
	{
	}
	const WPARAM keyCode;
};