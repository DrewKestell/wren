#include "stdafx.h"
#include "PlayerController.h"
#include "GameTimer.h"
#include "EventHandling/Events/MouseEvent.h"

extern unsigned int GetClientWidth();
extern unsigned int GetClientHeight();

// once you get the aspect ratio stuff figured out, you only need to calculate
// the centerPoint once (until the window gets resized)
PlayerController::PlayerController()
{
	clientWidth = GetClientWidth();
	clientHeight = GetClientHeight();
}

bool PlayerController::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::RightMouseDownEvent:
		{
			const auto derivedEvent = (MouseEvent*)event;

			isRightClickHeld = true;
			UpdateCurrentMouseDirection(derivedEvent->mousePosX, derivedEvent->mousePosY);

			break;
		}
		case EventType::RightMouseUpEvent:
		{
			const auto mouseUpEvent = (MouseEvent*)event;

			isRightClickHeld = false;

			break;
		}
		case EventType::MouseMoveEvent:
		{
			const auto derivedEvent = (MouseEvent*)event;

			if (isRightClickHeld)
				UpdateCurrentMouseDirection(derivedEvent->mousePosX, derivedEvent->mousePosY);

			break;
		}
	}

	return false;
}

// probably want a state machine here (moveState, etc)
void PlayerController::Update(GameTimer& gameTimer)
{
	if (isMoving)
	{
		auto pixelsToMove = MOVE_SPEED * gameTimer.DeltaTime();
		
		// if target is reached
		if (true)
			isMoving = false;
	}
	if (!isMoving && isRightClickHeld)
	{
		isMoving = true;
		currentMovementDirection = currentMouseDirection;
	}
}

void PlayerController::UpdateCurrentMouseDirection(float mousePosX, float mousePosY)
{
	auto centerPoint = XMVECTOR{ clientWidth / 2, clientHeight / 2, 0.0f, 0.0f };
	auto clickPoint = XMVECTOR{ mousePosX, mousePosY, 0.0f, 0.0f };
	auto clickVec = XMVectorSubtract(centerPoint, clickPoint);
	auto upVec = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
	auto angleVec = XMVectorATan2(XMVector3Dot(upVec, clickVec), XMVector3Cross(upVec, clickVec));
	XMFLOAT4 angleFloat;
	XMStoreFloat4(&angleFloat, angleVec);
	auto z = XMConvertToDegrees(angleFloat.z) + 180.0f;

	if (z >= 337.5 || z <= 22.5)
		currentMouseDirection = CardinalDirection::SouthWest;
	else if (z > 22.5 && z < 67.5)
		currentMouseDirection = CardinalDirection::South;
	else if (z > 67.5 && z < 112.5)
		currentMouseDirection = CardinalDirection::SouthEast;
	else if (z > 112.5 && z < 157.5)
		currentMouseDirection = CardinalDirection::East;
	else if (z > 157.5 && z < 202.5)
		currentMouseDirection = CardinalDirection::NorthEast;
	else if (z > 202.5 && z < 247.5)
		currentMouseDirection = CardinalDirection::North;
	else if (z > 247.5 && z < 292.5)
		currentMouseDirection = CardinalDirection::NorthWest;
	else
		currentMouseDirection = CardinalDirection::West;
}