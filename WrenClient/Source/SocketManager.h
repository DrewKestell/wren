#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <vector>

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

class SocketManager
{
private:
    sockaddr_in local;
    sockaddr_in to;
    int toLen;
    SOCKET socketC;   
    bool MessagePartsEqual(const char* first, const char* second, int length);
public:
    SocketManager();
    std::tuple<std::string, std::string, std::vector<std::string>*> TryRecieveMessage();
    void CloseSockets();
    void SendPacket(const std::string& opcode, const int argCount = 0, ...);
};