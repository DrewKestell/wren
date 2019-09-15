#pragma once

#include <SocketManager.h>
#include <OpCodes.h>
#include <CommonRepository.h>
#include "Components/PlayerComponent.h"
#include "ServerRepository.h"
#include <functional>

class ServerSocketManager : public SocketManager
{
    ServerRepository& repository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;
    sockaddr_in local;
    
	const bool ValidateToken(const int accountId, const std::string token); // this should probably return a PlayerComponent to improve performance
	PlayerComponent& GetPlayerComponent(const int accountId);
	void Login(const std::string& accountName, const std::string& password, const std::string& ipAndPort, sockaddr_in from);
	void CreateAccount(const std::string& accountName, const std::string& password, sockaddr_in from);
	void Logout(const int accountId);
	void CreateCharacter(const int accountId, const std::string& characterName);
	void UpdateLastHeartbeat(const int accountId);
	std::string ListCharacters(const int accountId);
	std::string ListSkills(const int accountId);
	std::string ListAbilities(const int characterId);
	void EnterWorld(const int accountId, const std::string& characterName);
	void DeleteCharacter(const int accountId, const std::string& characterName);
	void PropagateChatMessage(const std::string& senderName, const std::string& message);
	void ActivateAbility(PlayerComponent& player, Ability& ability);
	virtual void InitializeMessageHandlers();

public:
    ServerSocketManager(ServerRepository& repository, CommonRepository& commonRepository);
	void HandleTimeout();
	void UpdateClients();
	void SendPacket(const OpCode opCode, std::string args[], const int argCount);
	void SendPacket(sockaddr_in from, const OpCode opCode);
	void SendPacket(sockaddr_in from, const OpCode opCode, std::string args[], const int argCount);
};
