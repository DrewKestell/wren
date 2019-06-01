#pragma once

#include <WinDef.h>
#include "SystemKeyPressEvent.h"

class SystemKeyDownEvent : public SystemKeyPressEvent
{
public:
	SystemKeyDownEvent(const WPARAM keyCode)
		: SystemKeyPressEvent(EventType::SystemKeyDown, keyCode)
	{
	}
};