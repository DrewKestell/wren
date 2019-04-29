#include "stdafx.h"
#include "PlayerController.h"
#include "EventHandling/Events/MouseEvent.h"

PlayerController::PlayerController(GameObject& player)
	: player{ player }
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
void PlayerController::Update(const float deltaTime)
{
	const auto playerPos = player.GetWorldPosition();

	if (isMoving)
	{
		// if target is reached
		auto deltaX = std::abs(playerPos.x - destination.x);
		auto deltaZ = std::abs(playerPos.z - destination.z);

		if (deltaX < 1.0f && deltaZ < 1.0f)
		{
			player.SetLocalPosition(XMFLOAT3{ destination.x, 0.0f, destination.z });
			if (!isRightClickHeld)
			{
				isMoving = false;
				player.SetMovementVector(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			}
			else
			{
				player.SetMovementVector(currentMouseDirection);
				SetDestination(playerPos);
			}

			//
		}
	}
	if (!isMoving && isRightClickHeld)
	{
		isMoving = true;
		player.SetMovementVector(currentMouseDirection);
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