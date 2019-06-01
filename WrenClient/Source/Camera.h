#pragma once

#include "GameTimer.h"

static const float ACCELERATION = 0.02f;
static const float MIN_SPEED = 0.02f;
static const float MAX_SPEED = 0.00005f;
static const float OFFSET_X = 500.0f;
static const float OFFSET_Y = 700.0f;
static const float OFFSET_Z = -500.0f;

class Camera
{
	float camX{ 0.0f };
	float camY{ 0.0f };
	float camZ{ 0.0f };
	float currentSpeed{ MIN_SPEED };
public:
	void Update(const XMFLOAT3 vec, const float deltaTime);
	void Translate(const XMFLOAT3 pos) { camX += pos.x; camY += pos.y; camZ += pos.z; }
	XMFLOAT3 GetPosition() const { return XMFLOAT3{ camX + OFFSET_X, camY + OFFSET_Y, camZ + OFFSET_Z }; }
	XMFLOAT3 GetPositionFromOrigin() const { return XMFLOAT3{ camX, camY, camZ }; }
};