#pragma once

#include <string>

class Ability
{
public:
	Ability(const int abilityId, const std::string& name, const std::string& description, const int spriteId, const bool toggled, const bool targeted)
		: abilityId{ abilityId },
		  name{ name },
		  description{ description },
		  spriteId{ spriteId },
		  toggled{ toggled },
		  targeted{ targeted }
	{
	}
	const int abilityId;
	const std::string name;
	const std::string description;
	const int spriteId;
	const bool toggled;
	const bool targeted;
};