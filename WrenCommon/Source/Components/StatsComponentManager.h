#pragma once

#include "StatsComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"

constexpr unsigned int MAX_STATSCOMPONENTS_SIZE = 100000;

class StatsComponentManager : public Observer
{
	std::map<int, int> idIndexMap;
	StatsComponent statsComponents[MAX_STATSCOMPONENTS_SIZE];
	int statsComponentIndex{ 0 };
	EventHandler& eventHandler;
	ObjectManager& objectManager;
public:
	StatsComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	StatsComponent& CreateStatsComponent(
		const int gameObjectId,
		const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
		const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina);
	void DeleteStatsComponent(const int statsComponentId);
	StatsComponent& GetStatsComponentById(const int statsComponentId);
	const bool HandleEvent(const Event* const event) override;
	~StatsComponentManager();
};