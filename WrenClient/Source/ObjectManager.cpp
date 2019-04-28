#include "stdafx.h"
#include "ObjectManager.h"

GameObject& ObjectManager::CreateGameObject()
{
	if (gameObjectIndex == MAX_GAMEOBJECTS_SIZE)
		throw std::exception("Max GameObjects exceeded!");

	gameObjects[gameObjectIndex].Reset();
	return gameObjects[gameObjectIndex++];
}

RenderComponent& ObjectManager::CreateRenderComponent()
{
	if (renderComponentIndex == MAX_RENDERCOMPONENTS_SIZE)
		throw std::exception("Max RenderComponents exceeded!");

	renderComponents[renderComponentIndex].Reset();
	return renderComponents[renderComponentIndex++];
}

void ObjectManager::DeleteGameObject(const unsigned int gameObjectId)
{
	// first delete the render component by 
	auto gameObject = gameObjects[gameObjectId];
	auto renderComponentId = gameObject.GetRenderComponentId();
	memcpy(&gameObjects[renderComponentId], &gameObjects[--renderComponentIndex], sizeof(RenderComponent));

	// next delete the game object
	
}

void ObjectManager::DeleteRenderComponent(const unsigned int renderComponentId)
{

}