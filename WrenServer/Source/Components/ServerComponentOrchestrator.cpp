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

AIComponentManager* ServerComponentOrchestrator::GetAIComponentManager() { return aiComponentManager; }
PlayerComponentManager* ServerComponentOrchestrator::GetPlayerComponentManager() { return playerComponentManager; }
SkillComponentManager* ServerComponentOrchestrator::GetSkillComponentManager() { return skillComponentManager; }
StatsComponentManager* ServerComponentOrchestrator::GetStatsComponentManager() { return statsComponentManager;
}