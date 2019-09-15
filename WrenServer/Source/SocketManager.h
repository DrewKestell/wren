#pragma once

#include <OpCodes.h>
#include <CommonRepository.h>
#include "Components/PlayerComponent.h"
#include "ServerRepository.h"
#include <functional>

class SocketManager
{
	std::vector<std::function<void(std::vector<std::string> args)>> messageHandlers;
	std::map<OpCode, int> opCodeIndexMap;
    ServerRepository& repository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;
    sockaddr_in local;
    sockaddr_in from;
    int fromlen;
    SOCKET socketS;

	const bool ValidateToken(const int accountId, const std::string token); // this should probably return a PlayerComponent to improve performance
	PlayerComponent& GetPlayerComponent(const int accountId);
	void Login(const char* accountName, const char* password, const std::string& ipAndPort, sockaddr_in from);
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
	void InitializeMessageHandlers();
	void InitializeMessageHandler(const OpCode opCode, int& index, std::function<void(std::vector<std::string> args)> function);
public:
    SocketManager(ServerRepository& repository, CommonRepository& commonRepository);
    bool TryRecieveMessage();
    void CloseSockets();
	void HandleTimeout();
	void UpdateClients();
	void SendPacket(const OpCode opCode, std::string args[], const int argCount);
	void SendPacket(sockaddr_in from, const OpCode opCode);
	void SendPacket(sockaddr_in from, const OpCode opCode, std::string args[], const int argCount);
};
