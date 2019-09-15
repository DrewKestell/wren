#pragma once

#include <OpCodes.h>
#include <Models/Skill.h>
#include <Models/Ability.h>

class SocketManager
{
private:
	std::vector<std::function<void(std::vector<std::string> args)>> messageHandlers;
	std::map<OpCode, int> opCodeIndexMap;
	int accountId{ -1 };
	std::string token{ "" };
    sockaddr_in local;
    sockaddr_in to;
	int toLen{ 0 };
    SOCKET socketC;

    std::vector<std::string*>* BuildCharacterVector(const std::string& characterString);
	std::vector<WrenCommon::Skill*>* BuildSkillVector(const std::string& skillString);
	std::vector<Ability*>* BuildAbilityVector(const std::string& abilityString);
	void InitializeMessageHandlers();
	void InitializeMessageHandler(const OpCode opCode, int& index, std::function<void(std::vector<std::string> args)> function);
public:
	SocketManager();
    bool TryRecieveMessage();
    void CloseSockets();
	void SendPacket(const OpCode opcode);
    void SendPacket(const OpCode opcode, std::string args[], const int argCount);
	bool Connected();
	void Logout();
};