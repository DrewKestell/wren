#pragma once

#include "SystemKeyPressEvent.h"

class SystemKeyUpEvent : public SystemKeyPressEvent
{
public:
	SystemKeyUpEvent(const SystemKey code)
		: SystemKeyPressEvent(EventType::SystemKeyUpEvent, code)
	{
	}
};