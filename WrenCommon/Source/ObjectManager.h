#pragma once

#include "GameObject.h"
#include "EventHandling/EventHandler.h"

static const unsigned int MAX_GAMEOBJECTS_SIZE = 100000;

class ObjectManager
{
	std::map<long, unsigned int> idIndexMap;
	GameObject gameObjects[MAX_GAMEOBJECTS_SIZE];
	unsigned int gameObjectIndex{ 0 };
public:
	void Update(const float deltaTime);
	GameObject& CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale, long id = 0);
	void DeleteGameObject(EventHandler& eventHandler, const long gameObjectId);
	GameObject& GetGameObjectById(long gameObjectId);
};