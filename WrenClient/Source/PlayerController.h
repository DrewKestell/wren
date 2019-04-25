#pragma once

#include "EventHandling/Observer.h"
#include "CardinalDirection.h"
#include "GameTimer.h"
#include "Model.h"
#include "Camera.h"
#include "PlayerUpdate.h"

const float MOVE_SPEED = 80.0f;
const int BUFFER_SIZE = 120;

class PlayerController : public Observer
{
	int idCounter{ 0 };
	std::unique_ptr<PlayerUpdate> playerUpdates[BUFFER_SIZE];
	float destinationX{ 0.0f };
	float destinationZ{ 0.0f };
	float clientWidth{ 0.0f };
	float clientHeight{ 0.0f };
	bool isRightClickHeld{ false };
	bool isMoving{ false };
	CardinalDirection currentMouseDirection{ CardinalDirection::North };
	CardinalDirection currentMovementDirection{ CardinalDirection::North };
	GameTimer& gameTimer;
	Camera& camera;

	void SetDestination(const XMFLOAT3 playerPos);
	void UpdateCurrentMouseDirection(const float mousePosX, const float mousePosY);
public:
	PlayerController(GameTimer& gameTimer, Camera& camera);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	void SetClientDimensions(const int width, const int height) { clientWidth = (float)width; clientHeight = (float)height; }
};