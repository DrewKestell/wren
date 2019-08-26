#pragma once

#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class Character
{
	const int id;
	const std::string name;
	const int accountId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;
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
public:
	Character(
		const int id,
		const std::string name,
		const int accountId,
		const XMFLOAT3 position,
		const int modelId,
		const int textureId,
		const int agility,
		const int strength,
		const int wisdom,
		const int intelligence,
		const int charisma,
		const int luck,
		const int endurance,
		const int health,
		const int maxHealth,
		const int mana,
		const int maxMana,
		const int stamina,
		const int maxStamina)
		: id{ id },
		  name{ name },
		  accountId{ accountId },
		  position{ position },
		  modelId{ modelId },
		  textureId{ textureId },
		  agility{ agility },
		  strength{ strength },
		  wisdom{ wisdom },
		  intelligence{ intelligence },
		  charisma{ charisma },
		  luck{ luck },
		  endurance{ endurance },
		  health{ health },
		  maxHealth{ maxHealth },
		  mana{ mana },
		  maxMana{ maxMana },
		  stamina{ stamina },
		  maxStamina{ maxStamina }
	{
	}
	
	const int GetId() const { return id; }
	const std::string GetName() const { return name; }
	const int GetAccountId() const { return accountId; }
	const XMFLOAT3 GetPosition() const { return position; }
	const int GetModelId() const { return modelId; }
	const int GetTextureId() const { return textureId; }
	const int GetAgility() const { return agility; }
	const int GetStrength() const { return strength; }
	const int GetWisdom() const { return wisdom; }
	const int GetIntelligence() const { return intelligence; }
	const int GetCharisma() const { return charisma; }
	const int GetLuck() const { return luck; }
	const int GetEndurance() const { return endurance; }
	const int GetHealth() const { return health; }
	const int GetMaxHealth() const { return maxHealth; }
	const int GetMana() const { return mana; }
	const int GetMaxMana() const { return maxMana; }
	const int GetStamina() const { return stamina; }
	const int GetMaxStamina() const { return maxStamina; }
};