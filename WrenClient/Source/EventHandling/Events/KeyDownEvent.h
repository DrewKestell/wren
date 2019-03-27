#pragma once

#include "KeyPressEvent.h"

class KeyDownEvent : public KeyPressEvent
{
public:
	KeyDownEvent(const wchar_t charCode)
		: KeyPressEvent(EventType::KeyDownEvent, charCode)
	{
	}
};