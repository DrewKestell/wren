#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "AIComponent.h"
#include "ObjectManager.h"
#include <GameMap/GameMap.h>

static constexpr unsigned int MAX_AICOMPONENTS_SIZE = 100000;

class AIComponentManager : public Observer
{
	std::map<unsigned int, unsigned int> idIndexMap;
	AIComponent aiComponents[MAX_AICOMPONENTS_SIZE];
	unsigned int aiComponentIndex{ 0 };
	ObjectManager& objectManager;
	GameMap& gameMap;
public:
	AIComponentManager(ObjectManager& objectManager, GameMap& gameMap);
	AIComponent& CreateAIComponent(const long gameObjectId);
	void DeleteAIComponent(const unsigned int aiComponentId);
	AIComponent& GetAIComponentById(const unsigned int aiComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~AIComponentManager();
};