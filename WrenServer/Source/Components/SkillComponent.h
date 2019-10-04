#pragma once

#include <Components/Component.h>
#include "GameObject.h"
#include "../Models/Skill.h"
#include <Models/Skill.h>

class SkillComponent : public Component
{
	std::vector<std::unique_ptr<WrenServer::Skill>> skills;

	friend class SkillComponentManager;
public:
};