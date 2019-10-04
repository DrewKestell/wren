#pragma once

#include <Components/Component.h>
#include "GameObject.h"

class PlayerComponent : public Component
{
	std::string token{ "" };
	std::string ipAndPort{ "" };
	sockaddr_in fromSockAddr;

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

	const std::string& GetToken() const;
	const std::string& GetIPAndPort() const;
	const sockaddr_in& GetFromSockAddr() const;
};