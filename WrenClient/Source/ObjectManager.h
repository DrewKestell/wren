#pragma once

#include <forward_list>

class GameObject;

class ObjectManager
{
	std::forward_list<GameObject*> gameObjects;
public:
	void RegisterGameObject(GameObject& gameObject) { gameObjects.push_front(&gameObject); }
	void DeleteGameObject(GameObject& gameObject) { gameObjects.remove(&gameObject); };
	void Draw();
};