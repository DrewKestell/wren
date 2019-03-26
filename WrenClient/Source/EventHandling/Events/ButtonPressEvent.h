#pragma once

#include <string>
#include "Event.h"

class ButtonPressEvent : public Event
{
public:
	ButtonPressEvent(const std::string& buttonId)
		: Event(EventType::ButtonPressEvent),
		buttonId{ buttonId }
	{
	}
	const std::string& buttonId;
};