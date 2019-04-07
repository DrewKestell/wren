#pragma once

#include "EventHandling/Observer.h"
#include "CardinalDirection.h"
#include "GameTimer.h"

// 60 pixels (1 tile) per second, 0.06 pixels per ms
const float MOVE_SPEED = 0.06;

class PlayerController : public Observer
{
	float clientWidth;
	float clientHeight;
	CardinalDirection currentMouseDirection;
	bool isRightClickHeld;
	bool isMoving;
	CardinalDirection currentMovementDirection;

	void UpdateCurrentMouseDirection(float mousePosX, float mousePosY);
public:
	PlayerController();
	virtual bool HandleEvent(const Event* event);
	void Update(GameTimer& gameTimer);
};