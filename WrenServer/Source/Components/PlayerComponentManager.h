#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include <GameMap/GameMap.h>
#include "PlayerComponent.h"
#include "ObjectManager.h"

static constexpr unsigned int MAX_PLAYERCOMPONENTS_SIZE = 10000;

class PlayerComponentManager : public Observer
{
	std::map<unsigned int, unsigned int> idIndexMap;
	PlayerComponent playerComponents[MAX_PLAYERCOMPONENTS_SIZE];
	unsigned int playerComponentIndex{ 0 };
	ObjectManager& objectManager;
	GameMap& gameMap;

	const XMFLOAT3 GetDestinationVector(const XMFLOAT3 rightMouseDownDir, const XMFLOAT3 playerPos) const;
public:
	PlayerComponentManager(ObjectManager& objectManager, GameMap& gameMap);
	PlayerComponent& CreatePlayerComponent(const long gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const DWORD lastHeartbeat);
	void DeletePlayerComponent(const unsigned int playerComponentId);
	PlayerComponent& GetPlayerComponentById(const unsigned int playerComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	PlayerComponent* GetPlayerComponents();
	const unsigned int GetPlayerComponentIndex();
	~PlayerComponentManager();
};