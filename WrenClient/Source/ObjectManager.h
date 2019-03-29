#pragma once

#include <forward_list>

class GameObject;

class ObjectManager
{
	std::forward_list<GameObject*> gameObjects;
public:
	void RegisterGameObject(GameObject& gameObject) { gameObjects.push_front(&gameObject); }
	void Draw();
};