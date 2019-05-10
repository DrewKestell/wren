#pragma once

#include <Models/Skill.h>

class SocketManager
{
private:
	std::string m_token{ "" };
    sockaddr_in local;
    sockaddr_in to;
    int toLen;
    SOCKET socketC;   
    bool MessagePartsEqual(const char* first, const char* second, int length);
    std::vector<std::string*>* BuildCharacterVector(std::string* characterString);
	std::vector<Skill*>* BuildSkillVector(std::string& skillString);
public:
	SocketManager();
    bool TryRecieveMessage();
    void CloseSockets();
    void SendPacket(const std::string& opcode, const int argCount = 0, ...);
	bool Connected();
};