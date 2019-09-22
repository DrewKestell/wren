#pragma once

#include <string>

class Ability
{
public:
	Ability(const int abilityId, const std::string& name, const int spriteId, const bool toggled, const bool targeted)
		: abilityId{ abilityId },
		  name{ name },
		  spriteId{ spriteId },
		  toggled{ toggled },
		  targeted{ targeted }
	{
	}
	const int abilityId;
	const std::string name;
	const int spriteId;
	const bool toggled;
	const bool targeted;
};