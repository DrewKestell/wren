#pragma once

#include "GameTimer.h"
#include "EventHandling/Observer.h"

const float ACCELERATION = 0.0005f;
const float MIN_SPEED = 0.0005f;
const float MAX_SPEED = 0.00001f;
const float OFFSET_X = 500.0f;
const float OFFSET_Y = 700.0f;
const float OFFSET_Z = -500.0f;

class Camera : public Observer
{
	float camX = 0.0f;
	float camY = 0.0f;
	float camZ = 0.0f;
	float currentSpeed = MIN_SPEED;
public:
	virtual bool HandleEvent(const Event* event);
	void Update(XMFLOAT3 vec, GameTimer& gameTimer);
	void Translate(XMFLOAT3 pos) { camX += pos.x; camY += pos.y; camZ += pos.z; }
	XMFLOAT3 GetPosition() { return XMFLOAT3{ camX + OFFSET_X, camY + OFFSET_Y, camZ + OFFSET_Z }; }
	XMFLOAT3 GetPositionFromOrigin() { return XMFLOAT3{ camX, camY, camZ }; }
};