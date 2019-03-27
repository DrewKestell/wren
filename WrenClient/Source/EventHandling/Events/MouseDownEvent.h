#pragma once

#include "MouseEvent.h"

class MouseDownEvent : public MouseEvent
{
public:
	MouseDownEvent(const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::MouseDownEvent, mousePosX, mousePosY)
	{
	}
};