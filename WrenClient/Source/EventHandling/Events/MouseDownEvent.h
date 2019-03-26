#pragma once

#include "Event.h"

class MouseDownEvent : public Event
{
public:
	const float mousePosX;
	const float mousePosY;
	MouseDownEvent(const float mousePosX, const float mousePosY)
		: Event(EventType::MouseDownEvent),
		mousePosX{ mousePosX },
		mousePosY{ mousePosY }
	{
	}
};