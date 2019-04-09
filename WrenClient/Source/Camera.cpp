#include "stdafx.h"
#include "Camera.h"
#include "Utility.h"
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
void Camera::Update(XMFLOAT3 player)
{
	auto deltaX = std::abs(player.x - camX);
	auto deltaZ = std::abs(player.z - camZ);
	if (deltaX < 0.0005f && deltaZ < 0.0005f)
	{
		camX = player.x;
		camZ = player.z;
	}
	if (player.x == camX && player.z == camZ)
	{
		if (currentSpeed > MIN_SPEED)
			currentSpeed -= ACCELERATION;
	}
	else
	{
		if (currentSpeed < MAX_SPEED)
			currentSpeed += ACCELERATION;

		auto delta = XMFLOAT3{ player.x - camX, player.y - camY, player.z - camZ };
		auto vec = XMFLOAT3{ delta.x * currentSpeed, delta.y * currentSpeed, delta.z * currentSpeed };
		Translate(vec);
	}
}