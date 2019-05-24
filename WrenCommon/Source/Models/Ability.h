#pragma once

#include <string>

class Ability
{
public:
	Ability(const int abilityId, const std::string name, const int spriteId)
		: abilityId{ abilityId },
		  name{ name },
		  spriteId{ spriteId }
	{
	}
	const int abilityId;
	const std::string name;
	const int spriteId;
};