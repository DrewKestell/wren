#include "stdafx.h"
#include "ObjectManager.h"
#include "GameObject.h"

GameObject* ObjectManager::FindGameObject(void* ptr) const
{
	for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
	{
		if ((*it) == ptr)
			return (*it);
	}
	return nullptr;
}

void ObjectManager::Draw() const
{
	for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
		(*it)->Draw();
}