#pragma once

#include <string>
#include "Event.h"
#include "../../UI/UIButton.h"

class ButtonPressEvent : public Event
{
public:
	ButtonPressEvent(const UIButton* button)
		: Event(EventType::ButtonPressEvent),
		button{ button }
	{
	}
	const UIButton* button;
};