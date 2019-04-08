#pragma once

#include "GameTimer.h"
#include "EventHandling/Observer.h"

const float ACCELERATION = 0.002f;
const float MAX_SPEED = 0.06f;

class Camera : public Observer
{
	float camX = 500.0f;
	float camY = 650.0f;
	float camZ = -500.0f;
	float currentSpeed;
	float isMoving;
public:
	virtual bool HandleEvent(const Event* event);
	void Update(GameTimer& gameTimer);
	void Translate(XMFLOAT3 pos) { camX += pos.x; camY += pos.y; camZ += pos.z; }
	XMFLOAT3 GetPosition() { return XMFLOAT3{ camX, camY, camZ }; }
};