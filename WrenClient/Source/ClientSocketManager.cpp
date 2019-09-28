#include "stdafx.h"
#include "ClientSocketManager.h"
#include "Game.h"
#include "Events/AttackHitEvent.h"
#include "Events/AttackMissEvent.h"
#include "Events/SkillIncreaseEvent.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/EnterWorldSuccessEvent.h"
#include "EventHandling/Events/NpcUpdateEvent.h"
#include "EventHandling/Events/PlayerUpdateEvent.h"
#include "EventHandling/Events/PropagateChatMessageEvent.h"
#include "EventHandling/Events/ServerMessageEvent.h"
#include "EventHandling/Events/ActivateAbilitySuccessEvent.h"
#include "EventHandling/Events/NpcDeathEvent.h"

ClientSocketManager::ClientSocketManager(EventHandler& eventHandler)
	: eventHandler{ eventHandler }
{
	InitializeMessageHandlers();

    from.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_IP_ADDRESS, &from.sin_addr);
    from.sin_port = htons(SERVER_PORT_NUMBER);
}

void ClientSocketManager::SendPacket(const OpCode opCode)
{
	std::vector<std::string> args;
	SendPacket(opCode, args);
}

void ClientSocketManager::SendPacket(const OpCode opCode, std::vector<std::string>& args)
{
	if (Connected())
		args.insert(args.begin(), { std::to_string(accountId), token });

	SocketManager::SendPacket(from, opCode, args);
}

const bool ClientSocketManager::Connected() const
{
	return accountId != -1 && token != "";
}

std::vector<std::unique_ptr<std::string>> ClientSocketManager::BuildCharacterVector(const std::string& characterString) const
{
    std::vector<std::unique_ptr<std::string>> characterList;
	std::string arg{ "" };
    for (auto i = 0; i < characterString.length(); i++)
    {
        if (characterString.at(i) == ';')
        {
            characterList.push_back(std::make_unique<std::string>(arg));
			arg = "";
        }
        else
            arg += characterString.at(i);
    }
    return characterList;
}

std::vector<std::unique_ptr<WrenCommon::Skill>> ClientSocketManager::BuildSkillVector(const std::string& skillString) const
{
	std::vector<std::unique_ptr<WrenCommon::Skill>> skillList;
	char param = 0;
	std::string skillId{ "" };
	std::string name{ "" };
	std::string value{ "" };

	for (auto i = 0; i < skillString.length(); i++)
	{
		if (skillString.at(i) == ';')
		{
			skillList.push_back(std::make_unique<WrenCommon::Skill>(std::stoi(skillId), name, std::stoi(value)));
			param = 0;
			skillId = "";
			name = "";
			value = "";
		}
		else if (skillString.at(i) == '%')
				param++;
		else
		{
			if (param == 0)
				skillId += skillString.at(i);
			else if (param == 1)
				name += skillString.at(i);
			else
				value += skillString.at(i);
		}
	}

	return skillList;
}

std::vector<std::unique_ptr<Ability>> ClientSocketManager::BuildAbilityVector(const std::string& abilityString) const
{
	std::vector<std::unique_ptr<Ability>> abilityList;
	char param = 0;
	std::string abilityId{ "" };
	std::string name{ "" };
	std::string spriteId{ "" };
	std::string toggled{ "" };
	std::string targeted{ "" };

	for (auto i = 0; i < abilityString.length(); i++)
	{
		if (abilityString.at(i) == ';')
		{
			abilityList.push_back(std::make_unique<Ability>(std::stoi(abilityId), name, std::stoi(spriteId), toggled == "1", targeted == "1"));
			param = 0;
			abilityId = "";
			name = "";
			spriteId = "";
			toggled = "";
			targeted = "";
		}
		else if (abilityString.at(i) == '%')
			param++;
		else
		{
			if (param == 0)
				abilityId += abilityString.at(i);
			else if (param == 1)
				name += abilityString.at(i);
			else if (param == 2)
				spriteId += abilityString.at(i);
			else if (param == 3)
				toggled += abilityString.at(i);
			else
				targeted += abilityString.at(i);
		}
	}

	return abilityList;
}

std::vector<int> ClientSocketManager::BuildItemIdsVector(const std::string& itemIdsString) const
{
	std::vector<int> itemIdsList;
	std::string itemId{ "" };

	for (auto i = 0; i < itemIdsString.length(); i++)
	{
		if (itemIdsString.at(i) == ';')
		{
			itemIdsList.push_back(std::stoi(itemId));
			itemId = "";
		}
		else
		{
			itemId += itemIdsString.at(i);
		}
	}

	return itemIdsList;
}

void ClientSocketManager::Logout()
{
	accountId = -1;
	token = "";
}

void ClientSocketManager::InitializeMessageHandlers()
{
	messageHandlers[OpCode::CreateAccountFailure] = [this](const std::vector<std::string>& args)
	{
		const std::string& error = args.at(0);

		std::unique_ptr<Event> e = std::make_unique<CreateAccountFailedEvent>(error);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateAccountSuccess] = [this](const std::vector<std::string>& args)
	{
		std::unique_ptr<Event> e = std::make_unique<Event>(EventType::CreateAccountSuccess);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::LoginFailure] = [this](const std::vector<std::string>& args)
	{
		const std::string& error = args.at(0);
		
		std::unique_ptr<Event> e = std::make_unique<LoginFailedEvent>(error);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::LoginSuccess] = [this](const std::vector<std::string>& args)
	{
		const std::string& accountId = args.at(0);
		const std::string& token = args.at(1);
		const std::string& characterString = args.at(2);
		auto characterList = BuildCharacterVector(characterString);

		this->accountId = std::stoi(accountId);
		this->token = std::string{ token };

		std::unique_ptr<Event> e = std::make_unique<LoginSuccessEvent>(characterList);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateCharacterFailure] = [this](const std::vector<std::string>& args)
	{
		const std::string& error = args.at(0);

		std::unique_ptr<Event> e = std::make_unique<CreateCharacterFailedEvent>(error);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateCharacterSuccess] = [this](const std::vector<std::string>& args)
	{
		const std::string& characterString = args.at(0);
		auto characterList = BuildCharacterVector(characterString);

		std::unique_ptr<Event> e = std::make_unique<CreateCharacterSuccessEvent>(characterList);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::DeleteCharacterSuccess] = [this](const std::vector<std::string>& args)
	{
		const std::string& characterString = args.at(0);
		auto characterList = BuildCharacterVector(characterString);

		std::unique_ptr<Event> e = std::make_unique<DeleteCharacterSuccessEvent>(characterList);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::EnterWorldSuccess] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const std::string& accountId = args.at(j++);
		const std::string& positionX = args.at(j++);
		const std::string& positionY = args.at(j++);
		const std::string& positionZ = args.at(j++);
		const std::string& modelId = args.at(j++);
		const std::string& textureId = args.at(j++);
		auto skillVector = BuildSkillVector(args.at(j++));
		auto abilityVector = BuildAbilityVector(args.at(j++));
		const std::string& name = args.at(j++);
		const std::string& agility = args.at(j++);
		const std::string& strength = args.at(j++);
		const std::string& wisdom = args.at(j++);
		const std::string& intelligence = args.at(j++);
		const std::string& charisma = args.at(j++);
		const std::string& luck = args.at(j++);
		const std::string& endurance = args.at(j++);
		const std::string& health = args.at(j++);
		const std::string& maxHealth = args.at(j++);
		const std::string& mana = args.at(j++);
		const std::string& maxMana = args.at(j++);
		const std::string& stamina = args.at(j++);
		const std::string& maxStamina = args.at(j++);

		std::unique_ptr<Event> e = std::make_unique<EnterWorldSuccessEvent>
		(
			std::stoi(accountId),
			XMFLOAT3{ std::stof(positionX), std::stof(positionY), std::stof(positionZ) },
			std::stoi(modelId), std::stoi(textureId),
			skillVector, abilityVector,
			name,
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		);

		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::NpcUpdate] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const std::string& gameObjectId = args.at(j++);
		const std::string& posX = args.at(j++);
		const std::string& posY = args.at(j++);
		const std::string& posZ = args.at(j++);
		const std::string& movX = args.at(j++);
		const std::string& movY = args.at(j++);
		const std::string& movZ = args.at(j++);
		const std::string& agility = args.at(j++);
		const std::string& strength = args.at(j++);
		const std::string& wisdom = args.at(j++);
		const std::string& intelligence = args.at(j++);
		const std::string& charisma = args.at(j++);
		const std::string& luck = args.at(j++);
		const std::string& endurance = args.at(j++);
		const std::string& health = args.at(j++);
		const std::string& maxHealth = args.at(j++);
		const std::string& mana = args.at(j++);
		const std::string& maxMana = args.at(j++);
		const std::string& stamina = args.at(j++);
		const std::string& maxStamina = args.at(j++);

		std::unique_ptr<Event> e = std::make_unique<NpcUpdateEvent>
		(
			std::stol(gameObjectId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ)},
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		);

		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::PlayerUpdate] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const std::string& accountId = args.at(j++);
		const std::string& posX = args.at(j++);
		const std::string& posY = args.at(j++);
		const std::string& posZ = args.at(j++);
		const std::string& movX = args.at(j++);
		const std::string& movY = args.at(j++);
		const std::string& movZ = args.at(j++);
		const std::string& modelId = args.at(j++);
		const std::string& textureId = args.at(j++);
		const std::string& name = args.at(j++);
		const std::string& agility = args.at(j++);
		const std::string& strength = args.at(j++);
		const std::string& wisdom = args.at(j++);
		const std::string& intelligence = args.at(j++);
		const std::string& charisma = args.at(j++);
		const std::string& luck = args.at(j++);
		const std::string& endurance = args.at(j++);
		const std::string& health = args.at(j++);
		const std::string& maxHealth = args.at(j++);
		const std::string& mana = args.at(j++);
		const std::string& maxMana = args.at(j++);
		const std::string& stamina = args.at(j++);
		const std::string& maxStamina = args.at(j++);

		std::unique_ptr<Event> e = std::make_unique<PlayerUpdateEvent>
		(
			std::stol(accountId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ) },
			std::stoi(modelId), std::stoi(textureId),
			name,
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		);

		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::PropagateChatMessage] = [this](const std::vector<std::string>& args)
	{
		const std::string& message = args.at(0);
		const std::string& senderName = args.at(1);

		std::unique_ptr<Event> e = std::make_unique<PropagateChatMessageEvent>(senderName, message);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::ServerMessage] = [this](const std::vector<std::string>& args)
	{
		const std::string& message = args.at(0);
		const std::string& type = args.at(1);

		std::unique_ptr<Event> e = std::make_unique<ServerMessageEvent>(message, type);
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::AttackHit] = [this](const std::vector<std::string>& args)
	{
		const std::string& attackerId = args.at(0);
		const std::string& targetId = args.at(1);
		const std::string& damage = args.at(2);

		std::unique_ptr<Event> e = std::make_unique<AttackHitEvent>(std::stoi(attackerId), std::stoi(targetId), std::stoi(damage));
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::AttackMiss] = [this](const std::vector<std::string>& args)
	{
		const std::string& attackerId = args.at(0);
		const std::string& targetId = args.at(1);

		std::unique_ptr<Event> e = std::make_unique<AttackMissEvent>(std::stoi(attackerId), std::stoi(targetId));
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::ActivateAbilitySuccess] = [this](const std::vector<std::string>& args)
	{
		const std::string& abilityId = args.at(0);

		std::unique_ptr<Event> e = std::make_unique<ActivateAbilitySuccessEvent>(std::stoi(abilityId));
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::Pong] = [this](const std::vector<std::string>& args)
	{
		const std::string& pingId = args.at(0);

		game->OnPong(std::stoul(pingId));
	};

	messageHandlers[OpCode::SkillIncrease] = [this](const std::vector<std::string>& args)
	{
		const std::string& skillId = args.at(0);
		const std::string& newValue = args.at(1);

		std::unique_ptr<Event> e = std::make_unique<SkillIncreaseEvent>(std::stoi(skillId), std::stoi(newValue));
		eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::NpcDeath] = [this](const std::vector<std::string>& args)
	{
		const int gameObjectId = std::stoi(args.at(0));
		const std::vector<int> itemIds = BuildItemIdsVector(args.at(1));

		std::unique_ptr<Event> e = std::make_unique<NpcDeathEvent>(gameObjectId, itemIds);
		eventHandler.QueueEvent(e);
	};
}

void ClientSocketManager::SetGamePointer(Game* game) { this->game = game; }
