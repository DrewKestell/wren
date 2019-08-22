#pragma once

#include <Models/Skill.h>
#include <Models/Ability.h>

class SocketManager
{
private:
	std::string accountId{ "" };
	std::string token{ "" };
    sockaddr_in local;
    sockaddr_in to;
	int toLen{ 0 };
    SOCKET socketC;

    bool MessagePartsEqual(const char* first, const char* second, const int length) const;
    std::vector<std::string*>* BuildCharacterVector(std::string* characterString);
	std::vector<WrenCommon::Skill*>* BuildSkillVector(std::string& skillString);
	std::vector<Ability*>* BuildAbilityVector(std::string& abilityString);
public:
	SocketManager();
    bool TryRecieveMessage();
    void CloseSockets();
	void SendPacket(const std::string& opcode);
    void SendPacket(const std::string& opcode, std::string args[], const int argCount);
	bool Connected();
	void Logout();
};