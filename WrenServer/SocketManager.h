#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include "Player.h"
#include "Repository.h"

constexpr char CHECKSUM[8] = { '6', '5', '8', '3', '6', '2', '1', '6' };

constexpr char OPCODE_CONNECT[2] = { '0', '0' };
constexpr char OPCODE_DISCONNECT[2] = { '0', '1' };
constexpr char OPCODE_LOGIN_SUCCESSFUL[2] = { '0', '2' };
constexpr char OPCODE_LOGIN_UNSUCCESSFUL[2] = { '0', '3' };
constexpr char OPCODE_CREATE_ACCOUNT[2] = { '0', '4' };
constexpr char OPCODE_CREATE_ACCOUNT_SUCCESSFUL[2] = { '0', '5' };
constexpr char OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL[2] = { '0', '6' };
constexpr char OPCODE_CREATE_CHARACTER[2] = { '0', '7' };
constexpr char OPCODE_CREATE_CHARACTER_SUCCESSFUL[2] = { '0', '8' };
constexpr char OPCODE_CREATE_CHARACTER_UNSUCCESSFUL[2] = { '0', '9' };

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
    bool MessagePartsEqual(const char* first, const char* second, int length);
    void Login(
        const std::string& accountName,
        const std::string& password,
        const std::string& ipAndPort);
	void CreateAccount(const std::string& accountName, const std::string& password);	
	void Logout(const std::string& token);
	void CreateCharacter(const std::string& token, const std::string& characterName);
public:
    SocketManager(Repository& repository);
    void TryRecieveMessage();
    void CloseSockets();
    void HandleTimeout();
};

#endif