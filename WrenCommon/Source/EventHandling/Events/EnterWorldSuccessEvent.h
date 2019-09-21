#pragma once

#include <Models/Skill.h>
#include <Models/Ability.h>
#include "Event.h"
#include "../../Layer.h"
#include <DirectXMath.h>

using namespace DirectX;

class EnterWorldSuccessEvent : public Event
{
public:
	EnterWorldSuccessEvent(
		const int accountId,
		const XMFLOAT3 position,
		const int modelId, const int textureId,
		std::vector<std::unique_ptr<WrenCommon::Skill>>& skills, std::vector<std::unique_ptr<Ability>>& abilities,
		const std::string& name,
		const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
		const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina)
		: Event(EventType::EnterWorldSuccess),
		  accountId{ accountId },
		  position{ position },
		  modelId{ modelId }, textureId{ textureId },
		  skills{ std::move(skills) }, abilities{ std::move(abilities) },
		  name{ name },
		  agility{ agility }, strength{ strength }, wisdom{ wisdom }, intelligence{ intelligence }, charisma{ charisma }, luck{ luck }, endurance{ endurance },
		  health{ health }, maxHealth{ maxHealth }, mana{ mana }, maxMana{ maxMana }, stamina{ stamina }, maxStamina{ maxStamina }
	{
	}
	const int accountId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;
	std::vector<std::unique_ptr<WrenCommon::Skill>> skills;
	std::vector<std::unique_ptr<Ability>> abilities;
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