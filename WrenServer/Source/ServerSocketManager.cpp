#include "stdafx.h"
#include "ServerSocketManager.h"
#include "Components/AIComponentManager.h"
#include <Components/StatsComponentManager.h>
#include "Components/PlayerComponentManager.h"
#include "Components/SkillComponentManager.h"
#include "Components/InventoryComponentManager.h"

constexpr auto PLAYER_NOT_FOUND = "Player not found.";
constexpr auto INCORRECT_USERNAME = "Incorrect Username.";
constexpr auto INCORRECT_PASSWORD = "Incorrect Password.";
constexpr auto ACCOUNT_ALREADY_EXISTS = "Account already exists.";
constexpr auto LIBSODIUM_MEMORY_ERROR = "Ran out of memory while hashing password.";
constexpr auto CHARACTER_ALREADY_EXISTS = "Character already exists.";
constexpr auto INVALID_ATTACK_TARGET = "You can't attack that!";
constexpr auto DEAD_ATTACK_TARGET = "You can't attack something that is already dead.";
constexpr auto NO_ATTACK_TARGET = "You need a target before attacking!";
constexpr auto MESSAGE_TYPE_ERROR = "ERROR";
constexpr auto INVENTORY_FULL = "Inventory is full.";

ServerSocketManager::ServerSocketManager(
	EventHandler& eventHandler,
	GameMap& gameMap,
	ObjectManager& objectManager,
	ServerComponentOrchestrator& componentOrchestrator,
	ServerRepository& serverRepository,
	CommonRepository& commonRepository)
	: SocketManager{ eventHandler, SERVER_PORT_NUMBER },
	  gameMap{ gameMap },
	  objectManager{ objectManager },
	  componentOrchestrator{ componentOrchestrator },
	  serverRepository{ serverRepository },
	  commonRepository{ commonRepository }
{	
}

void ServerSocketManager::Initialize()
{
	InitializeMessageHandlers();

	// initialize LibSodium
	sodium_init();

	// initialize Abilities
	abilities = serverRepository.ListAbilities();

	// initialize StaticObjects
	auto staticObjects = commonRepository.ListStaticObjects();
	for (auto i = 0; i < staticObjects.size(); i++)
	{
		const StaticObject* staticObject = staticObjects.at(i).get();
		const auto pos = staticObject->GetPosition();
		const GameObject& gameObject = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, 0.0f, GameObjectType::StaticObject, staticObject->GetName(), staticObject->GetId(), true);
		gameMap.SetTileOccupied(gameObject.localPosition, true);
	}

	// initialize test dummy
	// we need to move ListNpcs to CommonRepository
	const std::string dummyName{ "Dummy1" };
	GameObject& dummyGameObject = objectManager.CreateGameObject(XMFLOAT3{ 30.0f, 0.0f, 30.0f }, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, 30.0f, GameObjectType::Npc, dummyName, 101, false, 2, 4);
	const auto dummyId = dummyGameObject.GetId();

	const auto aiComponentManager = componentOrchestrator.GetAIComponentManager();

	const AIComponent& dummyAIComponent = aiComponentManager->CreateAIComponent(dummyId);
	dummyGameObject.aiComponentId = dummyAIComponent.GetId();

	const auto statsComponentManager = componentOrchestrator.GetStatsComponentManager();
	const StatsComponent& dummyStatsComponent = statsComponentManager->CreateStatsComponent(dummyId, 10, 10, 10, 10, 10, 10, 10, 100, 100, 100, 100, 100, 100);
	dummyGameObject.statsComponentId = dummyStatsComponent.GetId();

	const auto inventoryComponentManager = componentOrchestrator.GetInventoryComponentManager();
	InventoryComponent& dummyInventoryComponent = inventoryComponentManager->CreateInventoryComponent(dummyId);
	dummyGameObject.inventoryComponentId = dummyInventoryComponent.GetId();
	dummyInventoryComponent.AddItem(1);
	dummyInventoryComponent.AddItem(2);
	gameMap.SetTileOccupied(dummyGameObject.localPosition, true);

	const std::string dummyName2{ "Dummy2" };
	GameObject& dummyGameObject2 = objectManager.CreateGameObject(XMFLOAT3{ 90.0f, 0.0f, 90.0f }, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, 30.0f, GameObjectType::Npc, dummyName2, 102, false, 2, 4);
	const auto dummyId2 = dummyGameObject2.GetId();

	const AIComponent& dummyAIComponent2 = aiComponentManager->CreateAIComponent(dummyId2);
	dummyGameObject2.aiComponentId = dummyAIComponent2.GetId();

	const StatsComponent& dummyStatsComponent2 = statsComponentManager->CreateStatsComponent(dummyId2, 10, 10, 10, 10, 10, 10, 10, 100, 100, 100, 100, 100, 100);
	dummyGameObject2.statsComponentId = dummyStatsComponent2.GetId();

	InventoryComponent& dummyInventoryComponent2 = inventoryComponentManager->CreateInventoryComponent(dummyId2);
	dummyGameObject2.inventoryComponentId = dummyInventoryComponent2.GetId();
	dummyInventoryComponent2.AddItem(2);
	dummyInventoryComponent2.AddItem(3);
	gameMap.SetTileOccupied(dummyGameObject2.localPosition, true);
}

void ServerSocketManager::SendPacket(const sockaddr_in& to, const OpCode opCode, const std::vector<std::string>& args)
{
	SocketManager::SendPacket(to, opCode, args);
}

void ServerSocketManager::SendPacketToAllClients(const OpCode opcode, const std::vector<std::string>& args)
{
	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto* const playerComponents = playerComponentManager->GetPlayerComponents();
	const auto playerComponentIndex = playerComponentManager->GetPlayerComponentIndex();

	for (auto i = 0; i < playerComponentIndex; i++)
	{
		SendPacket(playerComponents[i].GetFromSockAddr(), opcode, args);
	}
}

const bool ServerSocketManager::ValidateToken(const int accountId, const std::string token)
{
	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto gameObject = objectManager.GetGameObjectById(accountId);
	const auto playerComponent = playerComponentManager->GetComponentById(gameObject.playerComponentId);
	
	return token == playerComponent.GetToken();
}

PlayerComponent& ServerSocketManager::GetPlayerComponent(const int accountId)
{
	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto gameObject = objectManager.GetGameObjectById(accountId);

	return playerComponentManager->GetComponentById(gameObject.playerComponentId);
}

void ServerSocketManager::HandleTimeout()
{
	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto* const playerComponents = playerComponentManager->GetPlayerComponents();
	const auto playerComponentIndex = playerComponentManager->GetPlayerComponentIndex();

	for (auto i = 0; i < playerComponentIndex; i++)
	{
		const auto comp = playerComponents[i];
		if (GetTickCount64() > comp.lastHeartbeat + TIMEOUT_DURATION)
		{
			objectManager.DeleteGameObject(eventHandler, comp.GetGameObjectId());
		}
	}
}

void ServerSocketManager::Login(const std::string& accountName, const std::string& password, const std::string& ipAndPort, const sockaddr_in& from)
{
	std::string error;
	const auto account = serverRepository.GetAccount(accountName);
	if (account)
	{
		if (crypto_pwhash_str_verify(account->GetPassword().c_str(), password.c_str(), strlen(password.c_str())) != 0)
			error = INCORRECT_PASSWORD;
		else
		{
			GUID guid;
			if (FAILED(CoCreateGuid(&guid)))
				throw std::exception("Failed to create GUID.");
			char guid_cstr[39];
			snprintf(guid_cstr, sizeof(guid_cstr),
				"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
			const std::string token{ guid_cstr };

			const auto accountId = account->GetId();
			const std::string name{ "" };
			GameObject& playerGameObject = objectManager.CreateGameObject(VEC_ZERO, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, PLAYER_SPEED, GameObjectType::Player, name, accountId);
			const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
			const PlayerComponent& playerComponent = playerComponentManager->CreatePlayerComponent(playerGameObject.GetId(), token, ipAndPort, from, GetTickCount64());
			playerGameObject.playerComponentId = playerComponent.GetId();
            
			std::vector<std::string> args{ std::to_string(accountId), token, ListCharacters(accountId) };
			SendPacket(from, OpCode::LoginSuccess, args);
		}
	}
	else
		error = INCORRECT_USERNAME;

	if (error != "")
	{
		std::vector<std::string> args{ error };
		SendPacket(from, OpCode::LoginFailure, args);
	}
}

void ServerSocketManager::Logout(const int accountId)
{
	objectManager.DeleteGameObject(eventHandler, accountId);
}

void ServerSocketManager::CreateAccount(const std::string& accountName, const std::string& password, const sockaddr_in& from)
{
	if (serverRepository.AccountExists(accountName))
	{
		std::vector<std::string> args{ ACCOUNT_ALREADY_EXISTS };
		SendPacket(from, OpCode::CreateAccountFailure, args);
	}
	else
	{
		char hashedPassword[crypto_pwhash_STRBYTES];
		const auto passwordArr = password.c_str();
		const auto result = crypto_pwhash_str(
			hashedPassword,
			passwordArr,
			strlen(passwordArr),
			crypto_pwhash_OPSLIMIT_INTERACTIVE,
			crypto_pwhash_MEMLIMIT_INTERACTIVE);
		if (result != 0)
			throw std::exception(LIBSODIUM_MEMORY_ERROR);

		serverRepository.CreateAccount(accountName, std::string{ hashedPassword });
		SendPacket(from, OpCode::CreateAccountSuccess);
	}
}

void ServerSocketManager::CreateCharacter(const int accountId, const std::string& characterName)
{
	const PlayerComponent& playerComponent = GetPlayerComponent(accountId);

	if (serverRepository.CharacterExists(characterName))
	{
		std::vector<std::string> args{ CHARACTER_ALREADY_EXISTS };
		SendPacket(playerComponent.GetFromSockAddr(), OpCode::CreateCharacterFailure, args);
	}
	else
	{
		serverRepository.CreateCharacter(characterName, accountId);
		std::vector<std::string> args{ ListCharacters(accountId) };
		SendPacket(playerComponent.GetFromSockAddr(), OpCode::CreateCharacterSuccess, args);
	}
}

void ServerSocketManager::UpdateLastHeartbeat(const int accountId)
{
	PlayerComponent& playerComponent = GetPlayerComponent(accountId);
	playerComponent.lastHeartbeat = GetTickCount64();
}

std::string ServerSocketManager::ListCharacters(const int accountId)
{
	const auto characters = serverRepository.ListCharacters(accountId);
	std::string characterString{ "" };
    for (auto i = 0; i < characters.size(); i++)
        characterString += (characters.at(i) + ";");
    return characterString;
}

std::string ServerSocketManager::ListSkills(const int characterId)
{
	const auto skills = serverRepository.ListCharacterSkills(characterId);
	std::string skillString{ "" };
	for (auto i = 0; i < skills.size(); i++)
	{
		const auto skill = skills.at(i);
		skillString += (std::to_string(skill.skillId) + "%" + skill.name + "%" + std::to_string(skill.value) + ";");
	}
	return skillString;
}

std::string ServerSocketManager::ListAbilities(const int characterId)
{
	const auto abilities = serverRepository.ListCharacterAbilities(characterId);
	std::string abilityString{ "" };
	for (auto i = 0; i < abilities.size(); i++)
	{
		const auto ability = abilities.at(i);
		abilityString += (std::to_string(ability.abilityId) + "%" + ability.name + "%" + std::to_string(ability.spriteId) +  + "%" + std::to_string(ability.toggled) + "%" + std::to_string(ability.targeted) + ";");
	}
	return abilityString;
}

void ServerSocketManager::EnterWorld(const int accountId, const std::string& characterName)
{
	GameObject& gameObject = objectManager.GetGameObjectById(accountId);
	gameObject.name = characterName;

	PlayerComponent& playerComponent = GetPlayerComponent(accountId);
	const auto character = serverRepository.GetCharacter(characterName);
	gameObject.localPosition = character.GetPosition();
	playerComponent.lastHeartbeat = GetTickCount64();
	playerComponent.characterId = character.GetId();
	playerComponent.modelId = character.GetModelId();
	playerComponent.textureId = character.GetTextureId();

	const auto agility = character.GetAgility();
	const auto strength = character.GetStrength();
	const auto wisdom = character.GetWisdom();
	const auto intelligence = character.GetIntelligence();
	const auto charisma = character.GetCharisma();
	const auto luck = character.GetLuck();
	const auto endurance = character.GetEndurance();
	const auto health = character.GetHealth();
	const auto maxHealth = character.GetMaxHealth();
	const auto mana = character.GetMana();
	const auto maxMana = character.GetMaxMana();
	const auto stamina = character.GetStamina();
	const auto maxStamina = character.GetMaxStamina();

	const auto gameObjectId = gameObject.GetId();
	const auto statsComponentManager = componentOrchestrator.GetStatsComponentManager();
	const StatsComponent& statsComponent = statsComponentManager->CreateStatsComponent(
		gameObjectId,
		agility, strength, wisdom, intelligence, charisma, luck, endurance,
		health, maxHealth, mana, maxMana, stamina, maxStamina
	);
	gameObject.statsComponentId = statsComponent.GetId();

	auto skills = serverRepository.ListCharacterSkills(character.GetId());
	const auto skillComponentManager = componentOrchestrator.GetSkillComponentManager();
	const SkillComponent& skillComponent = skillComponentManager->CreateSkillComponent(gameObjectId, skills);
	gameObject.skillComponentId = skillComponent.GetId();

	const auto inventoryComponentManager = componentOrchestrator.GetInventoryComponentManager();
	const InventoryComponent& inventoryComponent = inventoryComponentManager->CreateInventoryComponent(gameObjectId);
	gameObject.inventoryComponentId = inventoryComponent.GetId();

	const auto pos = character.GetPosition();
	const auto charId = character.GetId();
	std::vector<std::string> args
	{
		std::to_string(accountId),
		std::to_string(pos.x), std::to_string(pos.y), std::to_string(pos.z),
		std::to_string(character.GetModelId()), std::to_string(character.GetTextureId()),
		ListSkills(charId), ListAbilities(charId),
		character.GetName(),
		std::to_string(agility), std::to_string(strength), std::to_string(wisdom), std::to_string(intelligence), std::to_string(charisma), std::to_string(luck), std::to_string(endurance),
		std::to_string(health), std::to_string(maxHealth), std::to_string(mana), std::to_string(maxMana), std::to_string(stamina), std::to_string(maxStamina),
	};
	SendPacket(playerComponent.GetFromSockAddr(), OpCode::EnterWorldSuccess, args);
	gameMap.SetTileOccupied(pos, true);
}

void ServerSocketManager::DeleteCharacter(const int accountId, const std::string& characterName)
{
	const PlayerComponent& playerComponent = GetPlayerComponent(accountId);
	serverRepository.DeleteCharacter(characterName);
	std::vector<std::string> args{ ListCharacters(accountId) };
	SendPacket(playerComponent.GetFromSockAddr(), OpCode::DeleteCharacterSuccess, args);
}

void ServerSocketManager::UpdateClients()
{
	const auto gameObjectLength = objectManager.GetGameObjectIndex();
	const auto* const gameObjects = objectManager.GetGameObjects();

	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto* const playerComponents = playerComponentManager->GetPlayerComponents();
	const auto playerComponentIndex = playerComponentManager->GetPlayerComponentIndex();

	const auto statsComponentManager = componentOrchestrator.GetStatsComponentManager();

	for (auto i = 0; i < playerComponentIndex; i++)
	{
		const PlayerComponent& playerToUpdate{ playerComponents[i] };

		// skip players that have logged in, but haven't selected a character and entered the game yet
		if (playerToUpdate.characterId == 0)
			continue;

		// for each player, send them an update for every non-static game object
		for (auto j = 0; j < gameObjectLength; j++)
		{
			const auto gameObject = gameObjects[j];

			const auto pos = gameObject.GetWorldPosition();
			const auto mov = gameObject.movementVector;

			const auto id = std::to_string(gameObject.GetId());
			const auto posX = std::to_string(pos.x);
			const auto posY = std::to_string(pos.y);
			const auto posZ = std::to_string(pos.z);
			const auto movX = std::to_string(mov.x);
			const auto movY = std::to_string(mov.y);
			const auto movZ = std::to_string(mov.z);

			const StatsComponent& stats = statsComponentManager->GetComponentById(gameObject.statsComponentId);
			const auto agility = std::to_string(stats.agility);
			const auto strength = std::to_string(stats.strength);
			const auto wisdom = std::to_string(stats.wisdom);
			const auto intelligence = std::to_string(stats.intelligence);
			const auto charisma = std::to_string(stats.charisma);
			const auto luck = std::to_string(stats.luck);
			const auto endurance = std::to_string(stats.endurance);
			const auto health = std::to_string(stats.health);
			const auto maxHealth = std::to_string(stats.maxHealth);
			const auto mana = std::to_string(stats.mana);
			const auto maxMana = std::to_string(stats.maxMana);
			const auto stamina = std::to_string(stats.stamina);
			const auto maxStamina = std::to_string(stats.maxStamina);

			const auto type = gameObject.GetType();
			if (type == GameObjectType::Npc)
			{
				std::vector<std::string> args{ id, posX, posY, posZ, movX, movY, movZ, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina };
				SendPacket(playerToUpdate.GetFromSockAddr(), OpCode::NpcUpdate, args);
			}
			else if (type == GameObjectType::Player)
			{
				const PlayerComponent& otherPlayer = playerComponentManager->GetComponentById(gameObject.playerComponentId);
				std::vector<std::string> args{ id, posX, posY, posZ, movX, movY, movZ, std::to_string(otherPlayer.modelId), std::to_string(otherPlayer.textureId), gameObject.name, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina };
				SendPacket(playerToUpdate.GetFromSockAddr(), OpCode::PlayerUpdate, args);
			}
		}
	}
}

void ServerSocketManager::PropagateChatMessage(const std::string& senderName, const std::string& message)
{
	const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();
	const auto* const playerComponents = playerComponentManager->GetPlayerComponents();
	const auto playerComponentIndex = playerComponentManager->GetPlayerComponentIndex();

	for (auto i = 0; i < playerComponentIndex; i++)
	{
		std::vector<std::string> args{ senderName, message };
		SendPacket(playerComponents[i].GetFromSockAddr(), OpCode::PropagateChatMessage, args);
	}
}

void ServerSocketManager::ActivateAbility(PlayerComponent& playerComponent, const Ability& ability)
{
	if (ability.name == "Auto Attack")
	{
		const GameObject& target = objectManager.GetGameObjectById(playerComponent.targetId);

		const auto statsComponentManager = componentOrchestrator.GetStatsComponentManager();
		const StatsComponent& targetStatsComponent = statsComponentManager->GetComponentById(target.statsComponentId);

		if (!playerComponent.autoAttackOn && playerComponent.targetId == -1)
		{
			std::vector<std::string> args{ NO_ATTACK_TARGET, MESSAGE_TYPE_ERROR };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ServerMessage, args);
			return;
		}
		else if (!playerComponent.autoAttackOn && objectManager.GetGameObjectById(playerComponent.targetId).isStatic)
		{
			std::vector<std::string> args{ INVALID_ATTACK_TARGET, MESSAGE_TYPE_ERROR };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ServerMessage, args);
			return;
		}
		else if (!playerComponent.autoAttackOn && !targetStatsComponent.alive)
		{
			std::vector<std::string> args{ DEAD_ATTACK_TARGET, MESSAGE_TYPE_ERROR };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ServerMessage, args);
			return;
		}
		else
		{
			playerComponent.autoAttackOn = !playerComponent.autoAttackOn;
		}
	}
	else if (ability.name == "Fireball")
	{

	}
	else if (ability.name == "Healing")
	{

	}

	std::vector<std::string> args{ std::to_string(ability.abilityId) };
	SendPacket(playerComponent.GetFromSockAddr(), OpCode::ActivateAbilitySuccess, args);
}

void ServerSocketManager::LootItem(const int accountId, const int gameObjectId, const int slot)
{
	// check if item exists, then move it from target inventory to player inventory, then send message to client
	const GameObject& gameObject = objectManager.GetGameObjectById(gameObjectId);
	const auto inventoryComponentManager = componentOrchestrator.GetInventoryComponentManager();
	InventoryComponent& inventoryComponent = inventoryComponentManager->GetComponentById(gameObject.inventoryComponentId);
	const auto itemId = inventoryComponent.itemIds.at(slot); // TODO: validate array bounds passed from client

	if (itemId >= 0)
	{
		const PlayerComponent& playerComponent = GetPlayerComponent(accountId);
		const GameObject& player = objectManager.GetGameObjectById(playerComponent.GetGameObjectId());
		InventoryComponent& playerInventoryComponent = inventoryComponentManager->GetComponentById(player.inventoryComponentId);

		const auto destinationSlot = playerInventoryComponent.AddItem(itemId);
		if (destinationSlot == -1)
		{
			std::vector<std::string> args{ INVENTORY_FULL, MESSAGE_TYPE_ERROR };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ServerMessage, args);
		}
		else
		{
			inventoryComponent.itemIds.at(slot) = -1;
			std::vector<std::string> args{ std::to_string(gameObjectId), std::to_string(slot), std::to_string(destinationSlot), std::to_string(itemId) };
			SendPacketToAllClients(OpCode::LootItemSuccess, args);
		}
	}
}

void ServerSocketManager::InitializeMessageHandlers()
{
	messageHandlers[OpCode::Connect] = [this](const std::vector<std::string>& args)
	{
		const std::string& accountName = args.at(0);
		const std::string& password = args.at(1);

		char str[INET_ADDRSTRLEN];
		ZeroMemory(str, sizeof(str));
		inet_ntop(AF_INET, &(from.sin_addr), str, INET_ADDRSTRLEN);
		const auto ipAndPort = std::string{ str } + ":" + std::to_string(from.sin_port);

		Login(accountName, password, ipAndPort, from);
	};

	messageHandlers[OpCode::Disconnect] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);

		ValidateToken(accountId, token);
		Logout(accountId);
	};
	
	messageHandlers[OpCode::CreateAccount] = [this](const std::vector<std::string>& args)
	{
		const std::string& accountName = args.at(0);
		const std::string& password = args.at(1);

		CreateAccount(accountName, password, from);
	};

	messageHandlers[OpCode::CreateCharacter] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const std::string& characterName = args.at(2);

		ValidateToken(accountId, token);
		CreateCharacter(accountId, characterName);
	};

	messageHandlers[OpCode::Heartbeat] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);

		ValidateToken(accountId, token);
		UpdateLastHeartbeat(accountId);
	};

	messageHandlers[OpCode::EnterWorld] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const std::string& characterName = args.at(2);

		ValidateToken(accountId, token);
		EnterWorld(accountId, characterName);
	};

	messageHandlers[OpCode::DeleteCharacter] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const std::string& characterName = args.at(2);

		ValidateToken(accountId, token);
		DeleteCharacter(accountId, characterName);
	};

	messageHandlers[OpCode::ActivateAbility] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const auto abilityId = std::stoi(args.at(2));

		ValidateToken(accountId, token);
		PlayerComponent& playerComponent = GetPlayerComponent(accountId);

		const auto abilityIt = find_if(abilities.begin(), abilities.end(), [&abilityId](Ability ability) { return ability.abilityId == abilityId; });
		if (abilityIt == abilities.end())
			return;

		ActivateAbility(playerComponent, *abilityIt);
	};

	messageHandlers[OpCode::SendChatMessage] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const std::string& message = args.at(2);
		const std::string& senderName = args.at(3);

		ValidateToken(accountId, token);
		PropagateChatMessage(senderName, message);
	};

	messageHandlers[OpCode::SetTarget] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const auto targetId = std::stol(args.at(2));

		ValidateToken(accountId, token);
		PlayerComponent& playerComponent = GetPlayerComponent(accountId);
		playerComponent.targetId = targetId;

		const GameObject& gameObject = objectManager.GetGameObjectById(targetId);

		// Toggle off Auto-Attack on the server and the client if the player switches to an invalid target.
		if (gameObject.isStatic && playerComponent.autoAttackOn)
		{
			playerComponent.autoAttackOn = false;
			std::vector<std::string> args1{ INVALID_ATTACK_TARGET, MESSAGE_TYPE_ERROR };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ServerMessage, args1);
			std::vector<std::string> args2{ "1" };
			SendPacket(playerComponent.GetFromSockAddr(), OpCode::ActivateAbilitySuccess, args2);
		}
	};

	messageHandlers[OpCode::UnsetTarget] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);

		ValidateToken(accountId, token);
		PlayerComponent& playerComponent{ GetPlayerComponent(accountId) };
		playerComponent.targetId = -1;
	};

	messageHandlers[OpCode::Ping] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const std::string& pingId = args.at(2);

		ValidateToken(accountId, token);

		const PlayerComponent& player = GetPlayerComponent(accountId);
		std::vector<std::string> outgoingArgs{ pingId };
		SendPacket(player.GetFromSockAddr(), OpCode::Pong, outgoingArgs);
	};

	messageHandlers[OpCode::PlayerRightMouseDown] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const auto dir = XMFLOAT3{ std::stof(args.at(2)), std::stof(args.at(3)), std::stof(args.at(4)) };

		ValidateToken(accountId, token);

		PlayerComponent& comp = GetPlayerComponent(accountId);
		comp.rightMouseDownDir = dir;
	};

	messageHandlers[OpCode::PlayerRightMouseUp] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);

		ValidateToken(accountId, token);

		PlayerComponent& comp = GetPlayerComponent(accountId);
		comp.rightMouseDownDir = VEC_ZERO;
	};

	messageHandlers[OpCode::PlayerRightMouseDirChange] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const auto dir = XMFLOAT3{ std::stof(args.at(2)), std::stof(args.at(3)), std::stof(args.at(4)) };

		ValidateToken(accountId, token);

		PlayerComponent& comp = GetPlayerComponent(accountId);
		comp.rightMouseDownDir = dir;
	};

	messageHandlers[OpCode::LootItem] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = std::stoi(args.at(0));
		const std::string& token = args.at(1);
		const auto gameObjectId = std::stoi(args.at(2));
		const auto slot = std::stoi(args.at(3));

		ValidateToken(accountId, token);

		LootItem(accountId, gameObjectId, slot);
	};
}
