#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "SkillComponent.h"
#include "ObjectManager.h"

static constexpr unsigned int MAX_SKILLCOMPONENTS_SIZE = 100000;

class SkillComponentManager : public Observer
{
	std::map<unsigned int, unsigned int> idIndexMap;
	SkillComponent skillComponents[MAX_SKILLCOMPONENTS_SIZE];
	unsigned int skillComponentIndex{ 0 };
	ObjectManager& objectManager;
public:
	SkillComponentManager(ObjectManager& objectManager);
	SkillComponent& CreateSkillComponent(const long gameObjectId, std::vector<WrenCommon::Skill>& skills);
	void DeleteSkillComponent(const unsigned int skillComponentId);
	SkillComponent& GetSkillComponentById(const unsigned int skillComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~SkillComponentManager();
};