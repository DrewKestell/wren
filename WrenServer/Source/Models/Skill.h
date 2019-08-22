#pragma once

namespace WrenServer
{
	class Skill
	{
	public:
		Skill(const int id, const int value)
			: id{ id },
			value{ value }
		{
		}
		const int id;
		int value;
	};
}
