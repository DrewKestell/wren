#include "stdafx.h"
#include "PlayerComponent.h"

void PlayerComponent::Initialize(const unsigned int id, const long gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
	this->token = token;
	this->ipAndPort = ipAndPort;
	this->fromSockAddr = fromSockAddr;
	this->lastHeartbeat = lastHeartbeat;
}