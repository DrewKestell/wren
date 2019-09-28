#include "stdafx.h"
#include "ServerComponentOrchestrator.h"

void ServerComponentOrchestrator::InitializeComponentManagers(
	AIComponentManager* aiComponentManager,
	PlayerComponentManager* playerComponentManager,
	SkillComponentManager* skillComponentManager,
	StatsComponentManager* statsComponentManager,
	InventoryComponentManager* inventoryComponentManager
)
{
	this->aiComponentManager = aiComponentManager;
	this->playerComponentManager = playerComponentManager;
	this->skillComponentManager = skillComponentManager;
	this->statsComponentManager = statsComponentManager;
	this->inventoryComponentManager = inventoryComponentManager;
}

AIComponentManager* ServerComponentOrchestrator::GetAIComponentManager() const { return aiComponentManager; }
PlayerComponentManager* ServerComponentOrchestrator::GetPlayerComponentManager() const { return playerComponentManager; }
SkillComponentManager* ServerComponentOrchestrator::GetSkillComponentManager() const { return skillComponentManager; }
StatsComponentManager* ServerComponentOrchestrator::GetStatsComponentManager() const { return statsComponentManager; }
InventoryComponentManager* ServerComponentOrchestrator::GetInventoryComponentManager() const { return inventoryComponentManager; }