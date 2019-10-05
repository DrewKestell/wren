#pragma once

#include <Components/Component.h>

class StatsComponent : public Component
{
	friend class StatsComponentManager;
public:
	int agility{ 0 };
	int strength{ 0 };
	int wisdom{ 0 };
	int intelligence{ 0 };
	int charisma{ 0 };
	int luck{ 0 };
	int endurance{ 0 };
	int health{ 0 };
	int maxHealth{ 0 };
	int mana{ 0 };
	int maxMana{ 0 };
	int stamina{ 0 };
	int maxStamina{ 0 };
	bool alive{ true };
};