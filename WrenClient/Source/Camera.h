#pragma once

#include "GameTimer.h"
#include "EventHandling/Observer.h"

const float ACCELERATION = 0.002f;
const float MAX_SPEED = 0.06f;

class Camera : public Observer
{
	float currentSpeed;
	float isMoving;
public:
	virtual bool HandleEvent(const Event* event);
	void Update(GameTimer& gameTimer);
};