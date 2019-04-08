#include "stdafx.h"
#include "PlayerController.h"
#include "EventHandling/Events/MouseEvent.h"

extern unsigned int GetClientWidth();
extern unsigned int GetClientHeight();

// once you get the aspect ratio stuff figured out, you only need to calculate
// the centerPoint once (until the window gets resized)
PlayerController::PlayerController(GameTimer& gameTimer, Model& player, Camera& camera)
	: gameTimer{ gameTimer },
	  player{ player },
	  camera{ camera }
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
void PlayerController::Update()
{
	if (isMoving)
	{
		auto pixelsToMove = MOVE_SPEED * gameTimer.DeltaTime();

		XMFLOAT3 vec{ 0.0f, 0.0f, 0.0f };

		if (currentMovementDirection == CardinalDirection::SouthWest)
		{
			if (playerPos.x >= 35.0f && playerPos.z >= 35.0f)
				vec = XMFLOAT3{ (float)-pixelsToMove / 2, 0.0f, (float)-pixelsToMove / 2 };
		}
		else if (currentMovementDirection == CardinalDirection::South)
		{
			if (playerPos.z >= 35.0f)
				vec = XMFLOAT3{ 0.0f, 0.0f, (float)-pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::SouthEast)
		{
			if (playerPos.x <= 2965.0f && playerPos.z >= 35.0f)
				vec = XMFLOAT3{ (float)pixelsToMove / 2, 0.0f, (float)-pixelsToMove / 2 };
		}
		else if (currentMovementDirection == CardinalDirection::East)
		{
			if (playerPos.x <= 2965.0f)
				vec = XMFLOAT3{ (float)pixelsToMove, 0.0f, 0.0f };
		}
		else if (currentMovementDirection == CardinalDirection::NorthEast)
		{
			if (playerPos.x <= 2965.0f && playerPos.z <= 2965.0f)
				vec = XMFLOAT3{ (float)pixelsToMove / 2, 0.0f, (float)pixelsToMove / 2 };
		}
		else if (currentMovementDirection == CardinalDirection::North)
		{
			if (playerPos.z <= 2965.0f)
				vec = XMFLOAT3{ 0.0f, 0.0f, (float)pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::NorthWest)
		{
			if (playerPos.x >= 35.0f && playerPos.z <= 2965.0f)
				vec = XMFLOAT3{ (float)-pixelsToMove / 2, 0.0f, (float)pixelsToMove / 2 };
		}
		else
		{
			if (playerPos.x >= 35.0f)
				vec = XMFLOAT3{ -pixelsToMove, 0.0f, 0.0f };
		}

		if (vec.x == 0.0f)
			return;

		player.Translate(XMMatrixTranslation(vec.x, vec.y, vec.z));
		camera.Translate(vec);

		// if target is reached
		if (false)
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

	auto playerPos = player.GetPosition();

	std::cout << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << "\n";

	if (z >= 337.5 || z <= 22.5)
	{
		if (playerPos.x >= 35.0f && playerPos.z >= 35.0f)
	}
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