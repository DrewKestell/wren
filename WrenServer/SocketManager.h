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

class SocketManager
{
private:
    Repository repository;
    std::vector<Player> players;
    sockaddr_in local;
    sockaddr_in from;
    int fromlen;
    SOCKET socketS;
    bool MessagePartsEqual(const char* first, const char* second, int length);
    void Login(
        const std::string& accountName,
        const std::string& password,
        const std::string characterName,
        const std::string ipAndPort,
        const char* inBuffer,
        char* outBuffer);
public:
    SocketManager(Repository& repository);
    void InitializeSockets();
    void TryRecieveMessage();
    void CloseSockets();
    void HandleTimeout();
};

#endif