#include "stdafx.h"
#include "ServerComponentOrchestrator.h"

void ServerComponentOrchestrator::InitializeComponentManagers(
	AIComponentManager* aiComponentManager,
	PlayerComponentManager* playerComponentManager,
	SkillComponentManager* skillComponentManager,
	StatsComponentManager* statsComponentManager
)
{
	this->aiComponentManager = aiComponentManager;
	this->playerComponentManager = playerComponentManager;
	this->skillComponentManager = skillComponentManager;
	this->statsComponentManager = statsComponentManager;
}

AIComponentManager* ServerComponentOrchestrator::GetAIComponentManager() const { return aiComponentManager; }
PlayerComponentManager* ServerComponentOrchestrator::GetPlayerComponentManager() const { return playerComponentManager; }
SkillComponentManager* ServerComponentOrchestrator::GetSkillComponentManager() const { return skillComponentManager; }
StatsComponentManager* ServerComponentOrchestrator::GetStatsComponentManager() const { return statsComponentManager;
}