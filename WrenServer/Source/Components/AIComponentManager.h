#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "AIComponent.h"
#include "ObjectManager.h"
#include <GameMap/GameMap.h>
#include "../ServerSocketManager.h"

constexpr unsigned int MAX_AICOMPONENTS_SIZE = 100000;

class AIComponentManager : public Observer
{
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	GameMap& gameMap;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
	std::map<int, int> idIndexMap;
	AIComponent aiComponents[MAX_AICOMPONENTS_SIZE];
	int aiComponentIndex{ 0 };
public:
	AIComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, GameMap& gameMap, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	AIComponent& CreateAIComponent(const int gameObjectId);
	void DeleteAIComponent(const int aiComponentId);
	AIComponent& GetAIComponentById(const int aiComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~AIComponentManager();
};