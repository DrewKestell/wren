#pragma once

#include <SocketManager.h>
#include <OpCodes.h>
#include <Models/Skill.h>
#include <Models/Ability.h>
#include <EventHandling/EventHandler.h>

class Game;

class ClientSocketManager : public SocketManager
{
private:
	EventHandler& eventHandler;
	Game* game;
	int accountId{ -1 };
	std::string token{ "" };

	std::vector<std::unique_ptr<std::string>> BuildCharacterVector(const std::string& characterString);
	std::vector<std::unique_ptr<WrenCommon::Skill>> BuildSkillVector(const std::string& skillString);
	std::vector<std::unique_ptr<Ability>> BuildAbilityVector(const std::string& abilityString);
	void InitializeMessageHandlers() override;
	
public:
	ClientSocketManager(EventHandler& eventHandler);
    
	void SetGamePointer(Game* game);
	void SendPacket(const OpCode opCode);
	void SendPacket(const OpCode opcode, std::vector<std::string>& args);
	const bool Connected() const;
	void Logout();
};