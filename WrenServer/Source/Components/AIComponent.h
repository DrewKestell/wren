#pragma once

#include "GameObject.h"

class AIComponent
{
	void Initialize(const unsigned int id, const long gameObjectId);

	friend class AIComponentManager;
public:
	unsigned int id{ 0 };
	long gameObjectId{ 0 };

	long targetId{ -1 };
	float swingTimer{ 0.0f };
};