#include "stdafx.h"
#include "PlayerController.h"
#include "EventHandling/Events/MouseEvent.h"

// once you get the aspect ratio stuff figured out, you only need to calculate
// the centerPoint once (until the window gets resized)
PlayerController::PlayerController(GameTimer& gameTimer, Model& player, Camera& camera)
	: gameTimer{ gameTimer },
	  player{ player },
	  camera{ camera }
{
	clientWidth = 800;
	clientHeight = 600;
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
	auto playerPos = player.GetPosition();
	//std::cout << "Player position: " << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << std::endl;
	camera.Update(player.GetPosition(), gameTimer);

	if (isMoving)
	{
		auto pixelsToMove = MOVE_SPEED * gameTimer.DeltaTime();

		XMFLOAT3 vec{ 0.0f, 0.0f, 0.0f };

		if (currentMovementDirection == CardinalDirection::SouthWest)
		{
			if (playerPos.x > 0.0f && playerPos.z > 0.0f)
				vec = XMFLOAT3{ -pixelsToMove, 0.0f, -pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::South)
		{
			if (playerPos.z > 0.0f)
				vec = XMFLOAT3{ 0.0f, 0.0f, -pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::SouthEast)
		{
			if (playerPos.x < 2970.0f && playerPos.z > 0.0f)
				vec = XMFLOAT3{ pixelsToMove, 0.0f, -pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::East)
		{
			if (playerPos.x < 2970.0f)
				vec = XMFLOAT3{ pixelsToMove, 0.0f, 0.0f };
		}
		else if (currentMovementDirection == CardinalDirection::NorthEast)
		{
			if (playerPos.x < 2970.0f && playerPos.z < 2970.0f)
				vec = XMFLOAT3{ pixelsToMove, 0.0f, pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::North)
		{
			if (playerPos.z < 2970.0f)
				vec = XMFLOAT3{ 0.0f, 0.0f, pixelsToMove };
		}
		else if (currentMovementDirection == CardinalDirection::NorthWest)
		{
			if (playerPos.x > 0.0f && playerPos.z < 2970.0f)
				vec = XMFLOAT3{ -pixelsToMove, 0.0f, pixelsToMove };
		}
		else
		{
			if (playerPos.x > 0.0f)
				vec = XMFLOAT3{ -pixelsToMove, 0.0f, 0.0f };
		}

		if (vec.x == 0.0f && vec.y == 0.0f && vec.z == 0.0f)
		{
			isMoving = false;
			return;
		}

		player.Translate(XMMatrixTranslation(vec.x, vec.y, vec.z));		

		// if target is reached
		auto deltaX = std::abs(playerPos.x - destinationX);
		auto deltaZ = std::abs(playerPos.z - destinationZ);

		/*std::cout << "deltaX: " << deltaX << std::endl;
		std::cout << "deltaZ: " << deltaZ << std::endl;*/

		if (deltaX < 0.2f && deltaZ < 0.2f)
		{
			player.SetPosition(XMFLOAT3{ destinationX, 0.0f, destinationZ });
			if (!isRightClickHeld)
			{
				isMoving = false;
			}
			else
			{
				currentMovementDirection = currentMouseDirection;
				SetDestination(playerPos);
			}
		}
	}
	if (!isMoving && isRightClickHeld)
	{
		isMoving = true;
		currentMovementDirection = currentMouseDirection;
		SetDestination(playerPos);
	}
}

void PlayerController::SetDestination(XMFLOAT3 playerPos)
{
	if (currentMovementDirection == CardinalDirection::SouthWest)
	{
		destinationX = playerPos.x - 60.0f;
		destinationZ = playerPos.z - 60.0f;
	}
	else if (currentMovementDirection == CardinalDirection::South)
	{
		destinationX = playerPos.x;
		destinationZ = playerPos.z - 60.0f;
	}
	else if (currentMovementDirection == CardinalDirection::SouthEast)
	{
		destinationX = playerPos.x + 60.0f;
		destinationZ = playerPos.z - 60.0f;
	}
	else if (currentMovementDirection == CardinalDirection::East)
	{
		destinationX = playerPos.x + 60.0f;
		destinationZ = playerPos.z;
	}
	else if (currentMovementDirection == CardinalDirection::NorthEast)
	{
		destinationX = playerPos.x + 60.0f;
		destinationZ = playerPos.z + 60.0f;
	}
	else if (currentMovementDirection == CardinalDirection::North)
	{
		destinationX = playerPos.x;
		destinationZ = playerPos.z + 60.0f;
	}
	else if (currentMovementDirection == CardinalDirection::NorthWest)
	{
		destinationX = playerPos.x - 60.0f;
		destinationZ = playerPos.z + 60.0f;
	}
	else
	{
		destinationX = playerPos.x - 60.0f;
		destinationZ = playerPos.z;
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
	{
		currentMouseDirection = CardinalDirection::SouthWest;
	}
	else if (z > 22.5 && z < 67.5)
	{
		currentMouseDirection = CardinalDirection::South;
	}
	else if (z > 67.5 && z < 112.5)
	{
		currentMouseDirection = CardinalDirection::SouthEast;
	}
	else if (z > 112.5 && z < 157.5)
	{
		currentMouseDirection = CardinalDirection::East;
	}
	else if (z > 157.5 && z < 202.5)
	{
		currentMouseDirection = CardinalDirection::NorthEast;
	}
	else if (z > 202.5 && z < 247.5)
	{
		currentMouseDirection = CardinalDirection::North;
	}
	else if (z > 247.5 && z < 292.5)
	{
		currentMouseDirection = CardinalDirection::NorthWest;
	}
	else
	{
		currentMouseDirection = CardinalDirection::West;
	}
}