#pragma once

#include <Constants.h>
#include "SkillComponent.h"
#include "../ServerSocketManager.h"
#include <Components/ComponentManager.h>

class SkillComponentManager : public ComponentManager<SkillComponent, 100000>
{
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;

public:
	SkillComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	SkillComponent& CreateSkillComponent(const int gameObjectId, std::vector<WrenCommon::Skill>& skills);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
};