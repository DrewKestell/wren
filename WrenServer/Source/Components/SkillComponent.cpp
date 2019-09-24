#include "stdafx.h"
#include "SkillComponent.h"

void SkillComponent::Initialize(const int id, const int gameObjectId, std::vector<WrenCommon::Skill>& skills)
{
	this->id = id;
	this->gameObjectId = gameObjectId;

	for (auto i = 0; i < skills.size(); i++)
	{
		const auto skill{ skills.at(i) };
		this->skills[skill.skillId] = new WrenServer::Skill(skill.skillId, skill.value);
	}
}

const int SkillComponent::GetId() const { return id; }
const int SkillComponent::GetGameObjectId() const { return gameObjectId; }