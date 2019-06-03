#include "stdafx.h"
#include <OpCodes.h>
#include "PlayerController.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/PlayerCorrectionEvent.h"

PlayerController::PlayerController(GameObject& player)
	: player{ player }
{
	clientWidth = 800;
	clientHeight = 600;
}

void PlayerController::OnPlayerCorrectionEvent(PlayerCorrectionEvent* event)
{
	const auto cachedInput = isRightClickHeld;
	const auto cachedMouseDir = currentMouseDirection;

	// first correct the incorrect position sent in the update
	const auto updateId = event->updateId;
	const auto correctedPos = XMFLOAT3{ event->posX, event->posY, event->posZ };
	playerUpdates[updateId % BUFFER_SIZE] = std::make_unique<PlayerUpdate>(updateId, correctedPos, playerUpdates[updateId % BUFFER_SIZE]->isRightClickHeld, playerUpdates[updateId % BUFFER_SIZE]->currentMouseDirection);
	player.localPosition = correctedPos;

	// now rerun the updates between the correction and current using the stored state
	for (auto i = updateId + 1; i < playerUpdateIdCounter; i++)
	{
		isRightClickHeld = playerUpdates[updateId % BUFFER_SIZE]->isRightClickHeld;
		currentMouseDirection = playerUpdates[updateId % BUFFER_SIZE]->currentMouseDirection;
		
		Update();
		player.Update();
	}

	isRightClickHeld = cachedInput;
	currentMouseDirection = cachedMouseDir;
}

void PlayerController::OnRightMouseDownEvent(MouseEvent* event)
{
	isRightClickHeld = true;
	UpdateCurrentMouseDirection(event->mousePosX, event->mousePosY);
}

void PlayerController::OnRightMouseUpEvent(MouseEvent* event)
{
	isRightClickHeld = false;
}

void PlayerController::OnMouseMoveEvent(MouseEvent* event)
{
	if (isRightClickHeld)
		UpdateCurrentMouseDirection(event->mousePosX, event->mousePosY);
}

// probably want a state machine here (moveState, etc)
void PlayerController::Update()
{
	const auto playerPos = player.GetWorldPosition();

	if (isMoving)
	{
		// if target is reached
		auto deltaX = std::abs(playerPos.x - destination.x);
		auto deltaZ = std::abs(playerPos.z - destination.z);

		if (deltaX < 1.0f && deltaZ < 1.0f)
		{
			player.localPosition = XMFLOAT3{ destination.x, 0.0f, destination.z };
			if (!isRightClickHeld)
			{
				isMoving = false;
				player.movementVector = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
			}
			else
			{
				player.movementVector = currentMouseDirection;
				SetDestination(playerPos);
			}
		}
	}
	if (!isMoving && isRightClickHeld)
	{
		isMoving = true;
		player.movementVector = currentMouseDirection;
		SetDestination(playerPos);
	}
}

void PlayerController::SetDestination(const XMFLOAT3 playerPos)
{
	const auto tileSize = 30.0f;
	const auto vec = XMLoadFloat3(&currentMouseDirection);
	const auto currentPos = XMLoadFloat3(&playerPos);
	XMStoreFloat3(&destination, currentPos + (vec * tileSize));
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
		currentMouseDirection = XMFLOAT3{ -1.0f, 0.0f, -1.0f };
	}
	else if (z > 22.5 && z < 67.5)
	{
		currentMouseDirection = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
	}
	else if (z > 67.5 && z < 112.5)
	{
		currentMouseDirection = XMFLOAT3{ 1.0f, 0.0f, -1.0f };
	}
	else if (z > 112.5 && z < 157.5)
	{
		currentMouseDirection = XMFLOAT3{ 1.0f, 0.0f, 0.0f };
	}
	else if (z > 157.5 && z < 202.5)
	{
		currentMouseDirection = XMFLOAT3{ 1.0f, 0.0f, 1.0f };
	}
	else if (z > 202.5 && z < 247.5)
	{
		currentMouseDirection = XMFLOAT3{ 0.0f, 0.0f, 1.0f };
	}
	else if (z > 247.5 && z < 292.5)
	{
		currentMouseDirection = XMFLOAT3{ -1.0f, 0.0f, 1.0f };
	}
	else
	{
		currentMouseDirection = XMFLOAT3{ -1.0f, 0.0f, 0.0f };
	}
}

PlayerUpdate* PlayerController::GeneratePlayerUpdate()
{
	const auto position = player.GetWorldPosition();

	playerUpdates[playerUpdateIdCounter % BUFFER_SIZE] = std::make_unique<PlayerUpdate>(playerUpdateIdCounter, position, isRightClickHeld, currentMouseDirection);

	playerUpdateIdCounter++;

	return playerUpdates[(playerUpdateIdCounter - 1) % BUFFER_SIZE].get();
}