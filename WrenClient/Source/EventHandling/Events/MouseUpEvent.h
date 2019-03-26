#pragma once

#include "Event.h"

class MouseUpEvent : public Event
{
public:
	MouseUpEvent()
		: Event(EventType::MouseUpEvent)
	{
	}
};