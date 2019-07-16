#pragma once

#include "GameObject.h"

class AIComponent
{
	void Initialize(const unsigned int id, const long gameObjectId);

	friend class AIComponentManager;
public:
	unsigned int id{ 0 };
	long gameObjectId{ 0 };

	bool isMoving{ false };
	XMFLOAT3 destination{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 movementVector{ 0.0f, 0.0f, 0.0f };
};