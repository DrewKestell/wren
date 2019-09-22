#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "SkillComponent.h"
#include "ObjectManager.h"
#include "../ServerSocketManager.h"

constexpr unsigned int MAX_SKILLCOMPONENTS_SIZE = 100000;

class SkillComponentManager : public Observer
{
	std::map<int, int> idIndexMap;
	SkillComponent skillComponents[MAX_SKILLCOMPONENTS_SIZE];
	int skillComponentIndex{ 0 };
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
public:
	SkillComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	SkillComponent& CreateSkillComponent(const int gameObjectId, std::vector<WrenCommon::Skill>& skills);
	void DeleteSkillComponent(const int skillComponentId);
	SkillComponent& GetSkillComponentById(const int skillComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~SkillComponentManager();
};