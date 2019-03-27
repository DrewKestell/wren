#pragma once

#include "MouseEvent.h"

class MouseUpEvent : public MouseEvent
{
public:
	MouseUpEvent(const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::MouseUpEvent, mousePosX, mousePosY)
	{
	}
};