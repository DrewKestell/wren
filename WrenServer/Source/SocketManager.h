#pragma once

#include <CommonRepository.h>
#include "Components/PlayerComponent.h"
#include "ServerRepository.h"

class SocketManager
{
private:
    ServerRepository& repository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;
    sockaddr_in local;
    sockaddr_in from;
    int fromlen;
    SOCKET socketS;

	const bool ValidateToken(const int accountId, const std::string token); // this should probably return a PlayerComponent to improve performance
	PlayerComponent& GetPlayerComponent(const int accountId);
	bool MessagePartsEqual(const char* first, const char* second, const int length);
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
public:
    SocketManager(ServerRepository& repository, CommonRepository& commonRepository);
    bool TryRecieveMessage();
    void CloseSockets();
	void HandleTimeout();
	void UpdateClients();
	void SendPacket(const std::string& opcode, std::string args[], const int argCount);
	void SendPacket(sockaddr_in from, const std::string& opcode);
	void SendPacket(sockaddr_in from, const std::string& opcode, std::string args[], const int argCount);
};
