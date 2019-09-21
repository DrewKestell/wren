#pragma once

#include "Event.h"

class PlayerUpdateEvent : public Event
{
public:
	PlayerUpdateEvent(
		const int accountId,
		const XMFLOAT3 pos,
		const XMFLOAT3 mov,
		const int modelId,
		const int textureId,
		const std::string& name,
		const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
		const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina)
		: Event(EventType::PlayerUpdate),
		accountId{ accountId },
		pos{ pos }, mov{ mov },
		modelId{ modelId }, textureId{ textureId },
		name{ name },
		agility{ agility }, strength{ strength }, wisdom{ wisdom }, intelligence{ intelligence }, charisma{ charisma }, luck{ luck }, endurance{ endurance },
		health{ health }, maxHealth{ maxHealth }, mana{ mana }, maxMana{ maxMana }, stamina{ stamina }, maxStamina{ maxStamina }
	{
	}
	const int accountId;
	const XMFLOAT3 pos;
	const XMFLOAT3 mov;
	const int modelId;
	const int textureId;
	const std::string name;
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