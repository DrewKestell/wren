#pragma once

#include <forward_list>
#include "GameObject.h"

class ObjectManager
{
	std::forward_list<GameObject&> gameObjects;
public:
	void RegisterGameObject(GameObject& gameObject) { gameObjects.push_front(gameObject); }
};