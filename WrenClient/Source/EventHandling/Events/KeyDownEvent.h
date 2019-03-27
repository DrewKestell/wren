#pragma once

#include "KeyPressEvent.h"

class KeyDownEvent : public KeyPressEvent
{
public:
	KeyDownEvent(const int charCode)
		: KeyPressEvent(EventType::KeyDownEvent, charCode)
	{
	}
};