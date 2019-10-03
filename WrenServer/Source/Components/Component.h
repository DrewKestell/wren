#pragma once

#include "GameObject.h"

class Component
{
protected:
	int id{ 0 };
	int gameObjectId{ 0 };

	template <class T, int maxComponents>
	friend class ComponentManager;
public:
	const int GetId() const;
	const int GetGameObjectId() const;
};