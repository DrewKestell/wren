#pragma once

#include "EventHandling/Observer.h"
#include "CardinalDirection.h"
#include "GameTimer.h"
#include "Model.h"
#include "Camera.h"

// 60 pixels (1 tile) per second, 0.06 pixels per ms
const float MOVE_SPEED = 100.0f;

class PlayerController : public Observer
{
	GameTimer& gameTimer;
	Model& player;
	Camera& camera;
	float clientWidth = 0.0f;
	float clientHeight = 0.0f;
	CardinalDirection currentMouseDirection;
	bool isRightClickHeld = false;
	bool isMoving = false;
	CardinalDirection currentMovementDirection;

	void UpdateCurrentMouseDirection(float mousePosX, float mousePosY);
public:
	PlayerController(GameTimer& gameTimer, Model& player, Camera& camera);
	virtual bool HandleEvent(const Event* event);
	void Update();
};