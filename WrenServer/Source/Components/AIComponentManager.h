#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "AIComponent.h"
#include "ObjectManager.h"
#include <GameMap/GameMap.h>

static constexpr unsigned int MAX_AICOMPONENTS_SIZE = 100000;

class AIComponentManager : public Observer
{
	std::map<int, int> idIndexMap;
	AIComponent aiComponents[MAX_AICOMPONENTS_SIZE];
	int aiComponentIndex{ 0 };
	ObjectManager& objectManager;
	GameMap& gameMap;
public:
	AIComponentManager(ObjectManager& objectManager, GameMap& gameMap);
	AIComponent& CreateAIComponent(const int gameObjectId);
	void DeleteAIComponent(const int aiComponentId);
	AIComponent& GetAIComponentById(const int aiComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~AIComponentManager();
};