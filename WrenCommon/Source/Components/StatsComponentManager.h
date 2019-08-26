#pragma once

#include "StatsComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"

static constexpr unsigned int MAX_STATSCOMPONENTS_SIZE = 100000;

class StatsComponentManager : public Observer
{
	std::map<unsigned int, unsigned int> idIndexMap;
	StatsComponent statsComponents[MAX_STATSCOMPONENTS_SIZE];
	unsigned int statsComponentIndex{ 0 };
	ObjectManager& objectManager;
public:
	StatsComponentManager(ObjectManager& objectManager);
	StatsComponent& CreateStatsComponent(const unsigned int gameObjectId, const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance, const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina);
	void DeleteStatsComponent(const unsigned int statsComponentId);
	StatsComponent& GetStatsComponentById(const unsigned int statsComponentId);
	virtual const bool HandleEvent(const Event* const event);
	~StatsComponentManager();
};