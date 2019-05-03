#pragma once

#include "Event.h"

class MouseEvent : public Event
{
public:
	const float mousePosX;
	const float mousePosY;
	MouseEvent(const EventType eventType, const float mousePosX, const float mousePosY)
		: Event(eventType),
		  mousePosX{ mousePosX },
		  mousePosY{ mousePosY }
	{
	}
};