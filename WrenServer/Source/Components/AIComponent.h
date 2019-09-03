#pragma once

#include "GameObject.h"

class AIComponent
{
	void Initialize(const int id, const int gameObjectId);

	friend class AIComponentManager;
public:
	int id{ 0 };
	int gameObjectId{ 0 };

	int targetId{ -1 };
	float swingTimer{ 0.0f };
};