#pragma once

#include <Socket.h>

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