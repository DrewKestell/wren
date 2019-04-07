#pragma once

class GameObject;

class ObjectManager
{
	std::forward_list<GameObject*> gameObjects;
public:
	void RegisterGameObject(GameObject& gameObject) { gameObjects.push_front(&gameObject); }
	void DeleteGameObject(GameObject& gameObject) { gameObjects.remove(&gameObject); };
	GameObject* FindGameObject(void* ptr)
	{
		for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
		{
			if ((*it) == ptr)
				return (*it);
		}
		return nullptr;
	}
	void Draw();
};