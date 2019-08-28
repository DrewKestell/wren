#pragma once

#include "GameObject.h"
#include "../Models/Skill.h"
#include <Models/Skill.h>

class SkillComponent
{
	int id{ 0 };
	int gameObjectId{ 0 };

	static constexpr int SKILL_COUNT = 100; // TODO: this count should probably come from the db
	WrenServer::Skill* skills[SKILL_COUNT]; // TODO: records from db have PKs starting from 1, so the first element at index 0 will be empty

	void Initialize(const int id, const int gameObjectId, std::vector<WrenCommon::Skill>& skills);

	friend class SkillComponentManager;
public:
	const int GetId() const;
	const int GetGameObjectId() const;
};