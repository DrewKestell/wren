#pragma once

#include <PlayerController.h>
#include "GameObject.h"

class PlayerComponent
{
	void Initialize(const unsigned int id, const long gameObjectId, const int accountId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const DWORD lastHeartbeat);

	friend class PlayerComponentManager;
public:
	unsigned int id{ 0 };
	long gameObjectId{ 0 };
	int accountId{ 0 };
	std::string token{ "" };
	std::string ipAndPort{ "" };
	sockaddr_in fromSockAddr;
	int updateCounter{ 0 };
	DWORD lastHeartbeat{ 0 };
	
	int characterId{ 0 };
	std::string characterName{ "" };
	int modelId{ 0 };
	int textureId{ 0 };
	std::unique_ptr<PlayerController> playerController;
	long targetId{ -1 };
	bool autoAttackOn{ false };
	float swingTimer{ 0.0f };
};