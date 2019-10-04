#pragma once

#include <Constants.h>
#include "AIComponent.h"
#include <GameMap/GameMap.h>
#include "../ServerSocketManager.h"
#include <Components/ComponentManager.h>

class AIComponentManager : public ComponentManager<AIComponent, 100000>
{
	GameMap& gameMap;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
public:
	AIComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, GameMap& gameMap, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	AIComponent& CreateAIComponent(const int gameObjectId);
	void Update();
};