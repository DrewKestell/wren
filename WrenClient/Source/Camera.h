#pragma once

#include "GameTimer.h"
#include "EventHandling/Observer.h"

const float ACCELERATION = 0.005f;
const float MAX_SPEED = 1.0f;

class Camera : public Observer
{
	float camX = 500.0f;
	float camY = 650.0f;
	float camZ = -500.0f;
	float currentSpeed = 0.0f;
	float isMoving = false;
public:
	virtual bool HandleEvent(const Event* event);
	void Update(XMFLOAT3 vec);
	void Translate(XMFLOAT3 pos) { camX += pos.x; camY += pos.y; camZ += pos.z; }
	XMFLOAT3 GetPosition() { return XMFLOAT3{ camX, camY, camZ }; }
	void Reset();
};