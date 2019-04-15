#pragma once

#include "Player.h"
#include "Repository.h"
#include <Socket.h>

class SocketManager
{
private:
    Repository repository;
    std::vector<Player*> players;
    sockaddr_in local;
    sockaddr_in from;
    int fromlen;
    SOCKET socketS;

	std::vector<Player*>::iterator GetPlayer(const std::string& token);
	void SendPacket(const std::string& opcode, const int argCount = 0, ...);
    bool MessagePartsEqual(const char* first, const char* second, const int length);
    void Login(const std::string& accountName, const std::string& password, const std::string& ipAndPort);
	void CreateAccount(const std::string& accountName, const std::string& password);	
	void Logout(const std::string& token);
	void CreateCharacter(const std::string& token, const std::string& characterName);
    void UpdateLastHeartbeat(const std::string& token);
    std::string ListCharacters(const int accountId);
    void EnterWorld(const std::string& token, const std::string& characterName);
	void DeleteCharacter(const std::string& token, const std::string& characterName);
public:
    SocketManager(Repository& repository);
    void TryRecieveMessage();
    void CloseSockets();
    void HandleTimeout();
};
