#pragma once

#include <Constants.h>
#include "PlayerComponent.h"
#include <GameMap/GameMap.h>
#include "../ServerSocketManager.h"
#include <Components/ComponentManager.h>

class PlayerComponentManager : public ComponentManager<PlayerComponent, 10000>
{
	GameMap& gameMap;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
	
	const XMFLOAT3 GetDestinationVector(const XMFLOAT3 rightMouseDownDir, const XMFLOAT3 playerPos) const;
public:
	PlayerComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, GameMap& gameMap, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	PlayerComponent& CreatePlayerComponent(const int gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat);
	void Update();
	PlayerComponent* GetPlayerComponents();
	const int GetPlayerComponentIndex();
};