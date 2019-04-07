#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <vector>
#include "EventHandling/EventHandler.h"

const std::string CHECKSUM = "65836216";

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
constexpr char OPCODE_HEARTBEAT[2] = { '1', '0' };
constexpr char OPCODE_ENTER_WORLD[2] = { '1', '1' };
constexpr char OPCODE_ENTER_WORLD_SUCCESSFUL[2] = { '1', '2' };
constexpr char OPCODE_DELETE_CHARACTER[2] = { '1', '3' };
constexpr char OPCODE_DELETE_CHARACTER_SUCCESSFUL[2] = { '1', '4' };

class SocketManager
{
private:
    sockaddr_in local;
    sockaddr_in to;
    int toLen;
    SOCKET socketC;   
    bool MessagePartsEqual(const char* first, const char* second, int length);
    std::vector<std::string*>* BuildCharacterVector(std::string* characterString);
public:
	SocketManager();
    bool TryRecieveMessage();
    void CloseSockets();
    void SendPacket(const std::string& opcode, const int argCount = 0, ...);
};