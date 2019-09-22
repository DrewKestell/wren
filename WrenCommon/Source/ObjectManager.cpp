#include "stdafx.h"
#include "ObjectManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

void ObjectManager::Update()
{
	for (auto i = 0; i < gameObjectIndex; i++)
		gameObjects[i].Update();
}

// I think all created GameObjects on the server should be coming from a DB somewhere, so they should have an Id
// For now, make id optional, and default to the gameObjectIndex it one isn't passed in
// On the client, all object creation should be triggered by an event from the server,
// so we should have an id in that case.
GameObject& ObjectManager::CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale, const float speed, GameObjectType type, const std::string& name, const int id, const bool isStatic, const int modelId, const int textureId)
{
	if (gameObjectIndex == MAX_GAMEOBJECTS_SIZE)
		throw std::exception("Max GameObjects exceeded!");

	auto gameObjectId = id == 0 ? gameObjectIndex : id;
	gameObjects[gameObjectIndex].Initialize(gameObjectId, type, name, localPosition, scale, speed, isStatic, modelId, textureId);
	idIndexMap[gameObjectId] = gameObjectIndex;
	return gameObjects[gameObjectIndex++];
}

void ObjectManager::DeleteGameObject(EventHandler& eventHandler, const int gameObjectId)
{
	// first copy the last GameObject into the index that was deleted
	auto gameObjectToDeleteIndex = idIndexMap[gameObjectId];
	auto lastGameObjectIndex = --gameObjectIndex;
	memcpy(&gameObjects[gameObjectToDeleteIndex], &gameObjects[lastGameObjectIndex], sizeof(GameObject));

	// then update the index of the moved GameObject
	auto lastGameObjectId = gameObjects[gameObjectToDeleteIndex].id;
	idIndexMap[lastGameObjectId] = gameObjectToDeleteIndex;

	// publish event that other ComponentManagers can subscribe to so they can delete those Components
	std::unique_ptr<Event> e = std::make_unique<DeleteGameObjectEvent>(gameObjectId);
	eventHandler.QueueEvent(e);
}

GameObject& ObjectManager::GetGameObjectById(const int gameObjectId)
{
	const auto index = idIndexMap[gameObjectId];
	return gameObjects[index];
}

// TODO: is this safe? this assumes the first object will always be in use before calling this function
const bool ObjectManager::GameObjectExists(const int gameObjectId)
{
	return idIndexMap[gameObjectId] > 0;
}

GameObject* ObjectManager::GetGameObjects()
{
	return gameObjects;
}

const int ObjectManager::GetGameObjectIndex()
{
	return gameObjectIndex;
}