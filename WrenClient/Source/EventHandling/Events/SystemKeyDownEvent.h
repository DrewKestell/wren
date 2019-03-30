#pragma once

#include "SystemKeyPressEvent.h"

class SystemKeyDownEvent : public SystemKeyPressEvent
{
public:
	SystemKeyDownEvent(const WPARAM keyCode)
		: SystemKeyPressEvent(EventType::SystemKeyDownEvent, keyCode)
	{
	}
};