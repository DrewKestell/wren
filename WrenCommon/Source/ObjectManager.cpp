#include "stdafx.h"
#include "ObjectManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

void ObjectManager::Update(const float deltaTime, bool emulateHack)
{
	for (unsigned int i = 0; i < gameObjectIndex; i++)
		gameObjects[i].Update(deltaTime, emulateHack);
}

// I think all created GameObjects on the server should be coming from a DB somewhere, so they should have an Id
// For now, make id optional, and default to the gameObjectIndex it one isn't passed in
// On the client, all object creation should be triggered by an event from the server,
// so we should have an id in that case.
GameObject& ObjectManager::CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale, const long id)
{
	if (gameObjectIndex == MAX_GAMEOBJECTS_SIZE)
		throw std::exception("Max GameObjects exceeded!");

	auto gameObjectId = id == 0 ? gameObjectIndex : id;
	gameObjects[gameObjectIndex].Initialize(gameObjectId, localPosition, scale);
	idIndexMap[gameObjectId] = gameObjectIndex;
	return gameObjects[gameObjectIndex++];
}

void ObjectManager::DeleteGameObject(EventHandler& eventHandler, const long gameObjectId)
{
	// first copy the last GameObject into the index that was deleted
	auto gameObjectToDeleteIndex = idIndexMap[gameObjectId];
	auto lastGameObjectIndex = --gameObjectIndex;
	memcpy(&gameObjects[gameObjectToDeleteIndex], &gameObjects[lastGameObjectIndex], sizeof(GameObject));

	// then update the index of the moved RenderComponent
	auto lastGameObjectId = gameObjects[gameObjectToDeleteIndex].GetId();
	idIndexMap[lastGameObjectId] = gameObjectToDeleteIndex;

	// publish event that other ComponentManagers can subscribe to so they can delete those Components
	eventHandler.QueueEvent(new DeleteGameObjectEvent(gameObjectId));
}

GameObject& ObjectManager::GetGameObjectById(const long gameObjectId)
{
	const auto index = idIndexMap[gameObjectId];
	return gameObjects[index];
}

// TODO: is this safe? this assumes the first object will always be in use before calling this function
const bool ObjectManager::GameObjectExists(const long gameObjectId)
{
	return idIndexMap[gameObjectId] > 0;
}