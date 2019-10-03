#pragma once

#include "Component.h"
#include "GameObject.h"

class AIComponent : public Component
{
	void Initialize(const int id, const int gameObjectId);

	friend class AIComponentManager;
public:
	int targetId{ -1 };
	float swingTimer{ 0.0f };
};