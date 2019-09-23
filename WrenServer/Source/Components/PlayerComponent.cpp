#include "stdafx.h"
#include "PlayerComponent.h"

void PlayerComponent::Initialize(const int id, const int gameObjectId, const std::string& token, const std::string& ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
	this->token = token;
	this->ipAndPort = ipAndPort;
	this->fromSockAddr = fromSockAddr;
	this->lastHeartbeat = lastHeartbeat;
}

const int PlayerComponent::GetId() const { return id; }
const int PlayerComponent::GetGameObjectId() const { return gameObjectId; }
const std::string& PlayerComponent::GetToken() const { return token; }
const std::string& PlayerComponent::GetIPAndPort() const { return ipAndPort; }
const sockaddr_in& PlayerComponent::GetFromSockAddr() const { return fromSockAddr; }