#pragma once

#include <SocketManager.h>
#include <OpCodes.h>
#include <CommonRepository.h>
#include "ServerRepository.h"
#include "Components/ServerComponentOrchestrator.h"
#include <EventHandling/EventHandler.h>
#include <GameMap/GameMap.h>
#include <ObjectManager.h>
#include "Components/PlayerComponent.h"

class ServerSocketManager : public SocketManager
{
	EventHandler& eventHandler;
	GameMap& gameMap;
	ObjectManager& objectManager;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerRepository& serverRepository;
	CommonRepository& commonRepository;
	std::vector<Ability> abilities;

	const bool ValidateToken(const int accountId, const std::string token); // this should probably return a PlayerComponent to improve performance
	PlayerComponent& GetPlayerComponent(const int accountId);
	void Login(const std::string& accountName, const std::string& password, const std::string& ipAndPort, const sockaddr_in& from);
	void CreateAccount(const std::string& accountName, const std::string& password, const sockaddr_in& from);
	void Logout(const int accountId);
	void CreateCharacter(const int accountId, const std::string& characterName);
	void UpdateLastHeartbeat(const int accountId);
	std::string ListCharacters(const int accountId);
	std::string ListSkills(const int accountId);
	std::string ListAbilities(const int characterId);
	void EnterWorld(const int accountId, const std::string& characterName);
	void DeleteCharacter(const int accountId, const std::string& characterName);
	void PropagateChatMessage(const std::string& senderName, const std::string& message);
	void ActivateAbility(PlayerComponent& player, const Ability& ability);
	void LootItem(const int accountId, const int gameObjectId, const int slot);
	void InitializeMessageHandlers() override;

public:
	ServerSocketManager(
		EventHandler& eventHandler,
		GameMap& gameMap,
		ObjectManager& objectManager,
		ServerComponentOrchestrator& componentOrchestrator,
		ServerRepository& serverRepository,
		CommonRepository& commonRepository);

	void Initialize();
	void HandleTimeout();
	void UpdateClients();
	void SendPacket(const sockaddr_in& to, const OpCode opCode, const std::vector<std::string>& args = std::vector<std::string>{});
	void SendPacketToAllClients(const OpCode opCode, const std::vector<std::string>& args = std::vector<std::string>{});
};
