#pragma once

#include <string>

namespace WrenCommon
{
	class Skill
	{
	public:
		Skill(const int skillId, const std::string& name, const int value)
			: skillId{ skillId },
			  name{ name },
			  value{ value }
		{
		}
		const int skillId;
		const std::string name;
		int value;
	};
}
