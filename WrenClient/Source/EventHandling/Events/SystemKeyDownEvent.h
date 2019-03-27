#pragma once

#include "SystemKeyPressEvent.h"

class SystemKeyDownEvent : public SystemKeyPressEvent
{
public:
	SystemKeyDownEvent(const SystemKey code)
		: SystemKeyPressEvent(EventType::SystemKeyDownEvent, code)
	{
	}
};