#pragma once

#include "EventHandling/Observer.h"
#include "CardinalDirection.h"
#include "GameTimer.h"
#include "Model.h"
#include "Camera.h"

const float MOVE_SPEED = 80.0f;

class PlayerController : public Observer
{
	float destinationX = 0.0f;
	float destinationZ = 0.0f;
	GameTimer& gameTimer;
	Model& player;
	Camera& camera;
	float clientWidth = 0.0f;
	float clientHeight = 0.0f;
	CardinalDirection currentMouseDirection;
	bool isRightClickHeld = false;
	bool isMoving = false;
	CardinalDirection currentMovementDirection;

	void SetDestination(XMFLOAT3 playerPos);
	void UpdateCurrentMouseDirection(float mousePosX, float mousePosY);
public:
	PlayerController(GameTimer& gameTimer, Model& player, Camera& camera);
	virtual bool HandleEvent(const Event* event);
	void Update();
	void SetClientDimensions(int width, int height) { clientWidth = width; clientHeight = height; }
};