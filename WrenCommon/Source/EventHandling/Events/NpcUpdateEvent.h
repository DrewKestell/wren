#pragma once

#include "Event.h"
#include <GameObjectType.h>

class NpcUpdateEvent : public Event
{
public:
	NpcUpdateEvent(
		const int gameObjectId,
		const XMFLOAT3 pos,
		const XMFLOAT3 mov,
		const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
		const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina)
		: Event(EventType::NpcUpdate),
		  gameObjectId{ gameObjectId },
		  pos{ pos }, mov{ mov },
		  agility{ agility }, strength{ strength }, wisdom{ wisdom }, intelligence{ intelligence }, charisma{ charisma }, luck{ luck }, endurance{ endurance },
		  health{ health }, maxHealth{ maxHealth }, mana{ mana }, maxMana{ maxMana }, stamina{ stamina }, maxStamina{ maxStamina }
	{
	}
	const int gameObjectId;
	const XMFLOAT3 pos;
	const XMFLOAT3 mov;
	const int agility;
	const int strength;
	const int wisdom;
	const int intelligence;
	const int charisma;
	const int luck;
	const int endurance;
	const int health;
	const int maxHealth;
	const int mana;
	const int maxMana;
	const int stamina;
	const int maxStamina;
};