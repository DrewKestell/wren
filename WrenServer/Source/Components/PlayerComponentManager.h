#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include <EventHandling/EventHandler.h>
#include <ObjectManager.h>
#include <GameMap/GameMap.h>
#include "PlayerComponent.h"
#include "ServerComponentOrchestrator.h"
#include "../ServerSocketManager.h"

constexpr unsigned int MAX_PLAYERCOMPONENTS_SIZE = 10000;

class PlayerComponentManager : public Observer
{
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	GameMap& gameMap;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
	std::map<int, int> idIndexMap;
	PlayerComponent playerComponents[MAX_PLAYERCOMPONENTS_SIZE];
	int playerComponentIndex{ 0 };
	
	const XMFLOAT3 GetDestinationVector(const XMFLOAT3 rightMouseDownDir, const XMFLOAT3 playerPos) const;
public:
	PlayerComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, GameMap& gameMap, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	PlayerComponent& CreatePlayerComponent(const int gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat);
	void DeletePlayerComponent(const int playerComponentId);
	PlayerComponent& GetPlayerComponentById(const int playerComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	PlayerComponent* GetPlayerComponents();
	const int GetPlayerComponentIndex();
	~PlayerComponentManager();
};