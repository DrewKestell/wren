#pragma once

#include "GameObject.h"
#include "EventHandling/EventHandler.h"

static constexpr unsigned int MAX_GAMEOBJECTS_SIZE = 100000;

class ObjectManager
{
	std::map<int, int> idIndexMap;
	GameObject gameObjects[MAX_GAMEOBJECTS_SIZE];
	unsigned int gameObjectIndex{ 0 };
public:
	void Update();
	GameObject& CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale, const float speed, GameObjectType type, std::string* name, const int id = 0, const bool isStatic = false, const int modelId = -1, const int textureId = -1);
	void DeleteGameObject(EventHandler& eventHandler, const int gameObjectId);
	GameObject& GetGameObjectById(const int gameObjectId);
	const bool GameObjectExists(const int gameObjectId);
	GameObject* GetGameObjects();
	const int GetGameObjectIndex();
};