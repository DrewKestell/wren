#pragma once

#include "GameObject.h"

class PlayerComponent
{
	int id{ 0 };
	int gameObjectId{ 0 };
	std::string token{ "" };
	std::string ipAndPort{ "" };
	sockaddr_in fromSockAddr;

	void Initialize(const int id, const int gameObjectId, const std::string& token, const std::string& ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat);

	friend class PlayerComponentManager;
public:
	unsigned __int64 lastHeartbeat{ 0 };
	int characterId{ 0 };
	int modelId{ 0 };
	int textureId{ 0 };
	int targetId{ -1 };
	bool autoAttackOn{ false };
	float swingTimer{ 0.0f };
	XMFLOAT3 rightMouseDownDir{ VEC_ZERO };

	const int GetId() const;
	const int GetGameObjectId() const;
	const std::string& GetToken() const;
	const std::string& GetIPAndPort() const;
	const sockaddr_in& GetFromSockAddr() const;
};