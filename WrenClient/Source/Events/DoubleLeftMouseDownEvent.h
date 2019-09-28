#pragma once

#include <EventHandling/Events/MouseEvent.h>
#include <GameObject.h>

class DoubleLeftMouseDownEvent : public MouseEvent
{
public:
	DoubleLeftMouseDownEvent(const float mousePosX, const float mousePosY, const GameObject* clickedObject = nullptr)
		: MouseEvent(EventType::DoubleLeftMouseDown, mousePosX, mousePosY),
		  clickedObject{ clickedObject }
	{
	}
	const GameObject* clickedObject;
};