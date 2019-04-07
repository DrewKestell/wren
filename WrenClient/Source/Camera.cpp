#include "stdafx.h"
#include "Camera.h"

#include "EventHandling/Events/MouseEvent.h"

bool Camera::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::RightMouseDownEvent:
		{
			const auto derivedEvent = (MouseEvent*)event;



			break;
		}
		case EventType::RightMouseUpEvent:
		{
			const auto mouseUpEvent = (MouseEvent*)event;



			break;
		}
		case EventType::MouseMoveEvent:
		{
			const auto derivedEvent = (MouseEvent*)event;



			break;
		}
	}

	return false;
}

// probably want a state machine here (moveState, etc)
void Camera::Update(GameTimer& gameTimer)
{
	if (isMoving)
	{
	}
	else
	{

	}
}
