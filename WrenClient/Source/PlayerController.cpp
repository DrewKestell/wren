#include "stdafx.h"
#include "PlayerController.h"
#include "EventHandling/Events/MouseEvent.h"
#include "SocketManager.h"
#include <OpCodes.h>

extern SocketManager g_socketManager;

// once you get the aspect ratio stuff figured out, you only need to calculate
// the centerPoint once (until the window gets resized)
PlayerController::PlayerController(GameTimer& gameTimer, Camera& camera, GameObject& player)
	: gameTimer{ gameTimer },
	  camera{ camera },
	  player{ player }
{
	clientWidth = 800;
	clientHeight = 600;
}

const bool PlayerController::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::PlayerCorrection:
		{
			//todo
		}
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
	const auto playerPos = player.GetWorldPosition();
	const auto deltaTime = gameTimer.DeltaTime();

	camera.Update(player.GetWorldPosition(), gameTimer);

	if (isMoving)
	{
		auto pixelsToMove = MOVE_SPEED * deltaTime;

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

		player.Translate(vec);		

		// if target is reached
		auto deltaX = std::abs(playerPos.x - destinationX);
		auto deltaZ = std::abs(playerPos.z - destinationZ);

		if (deltaX < 1.0f && deltaZ < 1.0f)
		{
			player.SetLocalPosition(XMFLOAT3{ destinationX, 0.0f, destinationZ });
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

	if (g_socketManager.Connected())
	{
		std::string state = "";

		if (isMoving)
			state = "Moving";
		else
			state = "Idle";

		const auto position = player.GetWorldPosition();

		playerUpdates[idCounter % BUFFER_SIZE] = std::make_unique<PlayerUpdate>(idCounter, position, std::make_unique<std::string>(state), currentMovementDirection, deltaTime);

		std::string dir = "";

		switch (currentMovementDirection)
		{
		case CardinalDirection::East:
			dir = "East";
			break;
		case CardinalDirection::North:
			dir = "North";
			break;
		case CardinalDirection::NorthEast:
			dir = "NorthEast";
			break;
		case CardinalDirection::NorthWest:
			dir = "NorthWest";
			break;
		case CardinalDirection::South:
			dir = "South";
			break;
		case CardinalDirection::SouthEast:
			dir = "SouthEast";
			break;
		case CardinalDirection::SouthWest:
			dir = "SouthWest";
			break;
		case CardinalDirection::West:
			dir = "West";
			break;
		}

		g_socketManager.SendPacket(
			OPCODE_PLAYER_UPDATE,
			7,
			std::to_string(idCounter),
			std::to_string(position.x),
			std::to_string(position.y),
			std::to_string(position.z),
			state,
			dir,
			std::to_string(deltaTime));

		idCounter++;
	}
}

void PlayerController::SetDestination(const XMFLOAT3 playerPos)
{
	auto tileSize = 30.0f;

	if (currentMovementDirection == CardinalDirection::SouthWest)
	{
		destinationX = playerPos.x - tileSize;
		destinationZ = playerPos.z - tileSize;
	}
	else if (currentMovementDirection == CardinalDirection::South)
	{
		destinationX = playerPos.x;
		destinationZ = playerPos.z - tileSize;
	}
	else if (currentMovementDirection == CardinalDirection::SouthEast)
	{
		destinationX = playerPos.x + tileSize;
		destinationZ = playerPos.z - tileSize;
	}
	else if (currentMovementDirection == CardinalDirection::East)
	{
		destinationX = playerPos.x + tileSize;
		destinationZ = playerPos.z;
	}
	else if (currentMovementDirection == CardinalDirection::NorthEast)
	{
		destinationX = playerPos.x + tileSize;
		destinationZ = playerPos.z + tileSize;
	}
	else if (currentMovementDirection == CardinalDirection::North)
	{
		destinationX = playerPos.x;
		destinationZ = playerPos.z + tileSize;
	}
	else if (currentMovementDirection == CardinalDirection::NorthWest)
	{
		destinationX = playerPos.x - tileSize;
		destinationZ = playerPos.z + tileSize;
	}
	else
	{
		destinationX = playerPos.x - tileSize;
		destinationZ = playerPos.z;
	}
}

void PlayerController::UpdateCurrentMouseDirection(const float mousePosX, const float mousePosY)
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