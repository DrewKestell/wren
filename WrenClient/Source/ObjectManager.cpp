#include "stdafx.h"
#include "ObjectManager.h"
#include "GameObject.h"

void ObjectManager::Draw()
{
	for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
		(*it)->Draw();
}