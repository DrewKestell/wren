#pragma once

#include <OpCodes.h>
#include <Models/Skill.h>
#include <Models/Ability.h>

class ClientSocketManager : public SocketManager
{
private:
	int accountId{ -1 };
	std::string token{ "" };
    sockaddr_in local;
    sockaddr_in to;
	int toLen{ 0 };
    SOCKET socketC;

    std::vector<std::string*>* BuildCharacterVector(const std::string& characterString);
	std::vector<WrenCommon::Skill*>* BuildSkillVector(const std::string& skillString);
	std::vector<Ability*>* BuildAbilityVector(const std::string& abilityString);
	
public:
	SocketManager();
    
    void CloseSockets();
	void SendPacket(const OpCode opcode);
    void SendPacket(const OpCode opcode, std::string args[], const int argCount);
	bool Connected();
	void Logout();
};