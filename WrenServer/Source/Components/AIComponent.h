#pragma once

#include "GameObject.h"

class AIComponent
{
	int id{ 0 };
	int gameObjectId{ 0 };

	void Initialize(const int id, const int gameObjectId);

	friend class AIComponentManager;
public:
	int targetId{ -1 };
	float swingTimer{ 0.0f };

	const int GetId() const;
	const int GetGameObjectId() const;
};