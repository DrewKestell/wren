#pragma once

#include "GameObject.h"
#include "../Models/Skill.h"
#include <Models/Skill.h>

class SkillComponent
{
	unsigned int id{ 0 };
	unsigned int gameObjectId{ 0 };

	static constexpr unsigned int SKILL_COUNT = 100; // TODO: this count should probably come from the db
	WrenServer::Skill* skills[SKILL_COUNT]; // TODO: records from db have PKs starting from 1, so the first element at index 0 will be empty

	void Initialize(const unsigned int id, const long gameObjectId, std::vector<WrenCommon::Skill>& skills);

	friend class SkillComponentManager;
public:
	const unsigned int GetId() const;
	const unsigned int GetGameObjectId() const;
};