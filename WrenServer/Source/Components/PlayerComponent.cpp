#include "stdafx.h"
#include "PlayerComponent.h"

const std::string& PlayerComponent::GetToken() const { return token; }
const std::string& PlayerComponent::GetIPAndPort() const { return ipAndPort; }
const sockaddr_in& PlayerComponent::GetFromSockAddr() const { return fromSockAddr; }