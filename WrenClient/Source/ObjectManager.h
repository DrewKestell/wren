#pragma once

#include "GameObject.h"

static const unsigned int MAX_GAMEOBJECTS_SIZE = 100000;
static const unsigned int MAX_RENDERCOMPONENTS_SIZE = 100000;

class ObjectManager
{
	GameObject gameObjects[MAX_GAMEOBJECTS_SIZE];
	RenderComponent renderComponents[MAX_RENDERCOMPONENTS_SIZE];
	unsigned int gameObjectIndex{ 0 };
	unsigned int renderComponentIndex{ 0 };
public:
	GameObject& CreateGameObject();
	RenderComponent& CreateRenderComponent();
	void DeleteGameObject(const unsigned int gameObjectId);
	void DeleteRenderComponent(const unsigned int renderComponentId);
};