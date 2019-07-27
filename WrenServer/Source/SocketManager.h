#pragma once

#include <CommonRepository.h>
#include <PlayerController.h>
#include "Components/PlayerComponent.h"
#include "ServerRepository.h"

class SocketManager
{
private:
    ServerRepository& repository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;
	std::vector<PlayerController> playerControllers;
    sockaddr_in local;
    sockaddr_in from;
    int fromlen;
    SOCKET socketS;

	void SendPacket(sockaddr_in from, const std::string& opcode, const int argCount = 0, ...);
	bool MessagePartsEqual(const char* first, const char* second, const int length);
	void Login(const std::string& accountName, const std::string& password, const std::string& ipAndPort, sockaddr_in from);
	void CreateAccount(const std::string& accountName, const std::string& password, sockaddr_in from);
	void Logout(const std::string& token);
	void CreateCharacter(const std::string& token, const std::string& characterName);
	void UpdateLastHeartbeat(const std::string& token);
	std::string ListCharacters(const int accountId);
	std::string ListSkills(const int accountId);
	std::string ListAbilities(const int characterId);
	void EnterWorld(const std::string& token, const std::string& characterName);
	void DeleteCharacter(const std::string& token, const std::string& characterName);
	void PlayerUpdate(const std::string& token, const std::string& idCounter, const std::string& characterId, const std::string& posX, const std::string& posY, const std::string& posZ, const std::string& isRightClickHeld, const std::string& mouseDirX, const std::string& mouseDirY, const std::string& mouseDirZ);
	void PropagateChatMessage(const std::string& senderName, const std::string& message);
	void ActivateAbility(PlayerComponent* player, Ability& ability);
public:
    SocketManager(ServerRepository& repository, CommonRepository& commonRepository);
    bool TryRecieveMessage();
    void CloseSockets();
    void HandleTimeout();
	void UpdateClients();
};
