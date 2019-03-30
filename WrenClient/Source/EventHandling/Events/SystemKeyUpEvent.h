#pragma once

#include "SystemKeyPressEvent.h"

class SystemKeyUpEvent : public SystemKeyPressEvent
{
public:
	SystemKeyUpEvent(const WPARAM keyCode)
		: SystemKeyPressEvent(EventType::SystemKeyUpEvent, keyCode)
	{
	}
};