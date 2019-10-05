#pragma once

#include "StatsComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"
#include <Components/ComponentManager.h>

class StatsComponentManager : public ComponentManager<StatsComponent, 100000>
{
public:
	StatsComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	StatsComponent& CreateStatsComponent(
		const int gameObjectId,
		const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
		const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina);
	const bool HandleEvent(const Event* const event) override;
};
