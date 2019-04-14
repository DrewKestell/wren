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
void Camera::Update(XMFLOAT3 player, GameTimer& gameTimer)
{
	//std::cout << gameTimer.DeltaTime() << std::endl;

	auto deltaX = std::abs(player.x - camX);
	auto deltaZ = std::abs(player.z - camZ);
	auto max = std::max(deltaX, deltaZ);
	deltaX = (float)(deltaX / max);
	deltaZ = (float)(deltaZ / max);

	/*std::cout << "deltaX: " << deltaX << std::endl;
	std::cout << "deltaZ: " << deltaZ << std::endl;*/

	if (deltaX < 0.0005f && deltaZ < 0.0005f)
	{
		camX = player.x;
		camZ = player.z;
	}

	const auto deltaAccel = ACCELERATION * gameTimer.DeltaTime();
	if (player.x == camX && player.z == camZ)
	{
		if (currentSpeed > MIN_SPEED)
			currentSpeed -= std::max(MIN_SPEED, deltaAccel);
	}
	else
	{
		if (currentSpeed < MAX_SPEED)
			currentSpeed += std::min(MAX_SPEED, deltaAccel);

		auto delta = XMFLOAT3{ player.x - camX, player.y - camY, player.z - camZ };
		auto vec = XMFLOAT3{ delta.x * currentSpeed, delta.y * currentSpeed, delta.z * currentSpeed };
		Translate(vec);
	}
}