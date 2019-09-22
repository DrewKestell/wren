#pragma once

#include <SocketManager.h>
#include <OpCodes.h>
#include <CommonRepository.h>
#include "Components/PlayerComponent.h"
#include "ServerRepository.h"

class ServerSocketManager : public SocketManager
{
	ServerRepository& repository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;

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
	void ActivateAbility(PlayerComponent& player, const Ability& ability);
	void InitializeMessageHandlers() override;

public:
	ServerSocketManager(ServerRepository& repository, CommonRepository& commonRepository);

	void HandleTimeout();
	void UpdateClients();
	void SendPacket(sockaddr_in to, const OpCode opCode, const std::vector<std::string>& args = std::vector<std::string>{});
	void SendPacketToAllClients(const OpCode opCode, const std::vector<std::string>& args = std::vector<std::string>{});
};
