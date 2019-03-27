#pragma once

#include "MouseEvent.h"

class MouseMoveEvent : public MouseEvent
{
public:
	MouseMoveEvent(const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::MouseMoveEvent, mousePosX, mousePosY)
	{
	}
};
