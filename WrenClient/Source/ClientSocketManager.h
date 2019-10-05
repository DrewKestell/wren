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
	Game* game;
	int accountId{ -1 };
	std::string token{ "" };

	std::vector<std::unique_ptr<std::string>> BuildCharacterVector(const std::string& characterString) const;
	std::vector<std::unique_ptr<WrenCommon::Skill>> BuildSkillVector(const std::string& skillString) const;
	std::vector<std::unique_ptr<Ability>> BuildAbilityVector(const std::string& abilityString) const;
	std::vector<int> BuildItemIdsVector(const std::string& itemIdsString) const;
	void InitializeMessageHandlers() override;
	
public:
	ClientSocketManager(EventHandler& eventHandler);
    
	void SetGamePointer(Game* game);
	void SendPacket(const OpCode opCode);
	void SendPacket(const OpCode opcode, std::vector<std::string>& args);
	const bool Connected() const;
	void Logout();
};