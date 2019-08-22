#include "stdafx.h"
#include "SkillComponent.h"

void SkillComponent::Initialize(const unsigned int id, const long gameObjectId, std::vector<WrenCommon::Skill>& skills)
{
	this->id = id;
	this->gameObjectId = gameObjectId;

	for (auto i = 0; i < skills.size(); i++)
	{
		const auto skill = skills[i];
		this->skills[skill.skillId] = new WrenServer::Skill(skill.skillId, skill.value);
	}
}