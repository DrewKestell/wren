#pragma once

class AIComponentManager;
class PlayerComponentManager;
class SkillComponentManager;
class StatsComponentManager;

class ServerComponentOrchestrator
{
	AIComponentManager* aiComponentManager{ nullptr };
	PlayerComponentManager* playerComponentManager{ nullptr };
	SkillComponentManager* skillComponentManager{ nullptr };
	StatsComponentManager* statsComponentManager{ nullptr };

public:
	void InitializeComponentManagers(
		AIComponentManager* aiComponentManager,
		PlayerComponentManager* playerComponentManager,
		SkillComponentManager* skillComponentManager,
		StatsComponentManager* statsComponentManager
	);

	AIComponentManager* GetAIComponentManager();
	PlayerComponentManager* GetPlayerComponentManager();
	SkillComponentManager* GetSkillComponentManager();
	StatsComponentManager* GetStatsComponentManager();
};