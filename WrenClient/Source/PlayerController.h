#pragma once

#include "EventHandling/Observer.h"
#include "GameTimer.h"
#include "Camera.h"
#include "PlayerUpdate.h"
#include "GameObject.h"

class PlayerController : public Observer
{
	float clientWidth{ 0.0f };
	float clientHeight{ 0.0f };
	bool isRightClickHeld{ false };
	bool isMoving{ false };
	XMFLOAT3 currentMouseDirection{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 destination{ 0.0f, 0.0f, 0.0f };
	GameObject& player;

	void SetDestination(const XMFLOAT3 playerPos);
	void UpdateCurrentMouseDirection(const float mousePosX, const float mousePosY);
public:
	PlayerController(GameObject& player);
	virtual const bool HandleEvent(const Event* const event);
	void Update(const float deltaTime);
	void SetClientDimensions(const int width, const int height) { clientWidth = (float)width; clientHeight = (float)height; }
	~PlayerController();
};