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

extern EventHandler g_eventHandler;
extern std::unique_ptr<Game> g_game;

static bool logMessages{ false };

ClientSocketManager::ClientSocketManager()
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

std::vector<std::unique_ptr<std::string>> ClientSocketManager::BuildCharacterVector(const std::string& characterString)
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

std::vector<std::unique_ptr<WrenCommon::Skill>> ClientSocketManager::BuildSkillVector(const std::string& skillString)
{
	std::vector<std::unique_ptr<WrenCommon::Skill>> skillList;
	char param = 0;
	std::string skillId = "";
	std::string name = "";
	std::string value = "";

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

std::vector<std::unique_ptr<Ability>> ClientSocketManager::BuildAbilityVector(const std::string& abilityString)
{
	std::vector<std::unique_ptr<Ability>> abilityList;
	char param = 0;
	std::string abilityId = "";
	std::string name = "";
	std::string spriteId = "";
	std::string toggled = "";
	std::string targeted = "";

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

void ClientSocketManager::Logout()
{
	accountId = -1;
	token = "";
}

void ClientSocketManager::InitializeMessageHandlers()
{
	messageHandlers[OpCode::CreateAccountFailure] = [this](const std::vector<std::string>& args)
	{
		const auto error = args[0];

		std::unique_ptr<Event> e = std::make_unique<CreateAccountFailedEvent>(new std::string(error));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateAccountSuccess] = [this](const std::vector<std::string>& args)
	{
		std::unique_ptr<Event> e = std::make_unique<Event>(EventType::CreateAccountSuccess);
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::LoginFailure] = [this](const std::vector<std::string>& args)
	{
		const auto error = args[0];
		
		std::unique_ptr<Event> e = std::make_unique<LoginFailedEvent>(new std::string(error));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::LoginSuccess] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = args.at(0);
		const auto token = args.at(1);
		const auto characterString = args.at(2);
		auto characterList = BuildCharacterVector(characterString);

		this->accountId = std::stoi(accountId);
		this->token = std::string(token);

		std::unique_ptr<Event> e = std::make_unique<LoginSuccessEvent>(characterList);
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateCharacterFailure] = [this](const std::vector<std::string>& args)
	{
		const auto error = args[0];

		std::unique_ptr<Event> e = std::make_unique<CreateCharacterFailedEvent>(new std::string(error));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::CreateCharacterSuccess] = [this](const std::vector<std::string>& args)
	{
		const auto characterString = args[0];
		auto characterList = BuildCharacterVector(characterString);

		std::unique_ptr<Event> e = std::make_unique<CreateCharacterSuccessEvent>(characterList);
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::DeleteCharacterSuccess] = [this](const std::vector<std::string>& args)
	{
		const auto characterString = args[0];
		auto characterList = BuildCharacterVector(characterString);

		std::unique_ptr<Event> e = std::make_unique<DeleteCharacterSuccessEvent>(characterList);
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::EnterWorldSuccess] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const auto accountId = args.at(j++);
		const auto positionX = args.at(j++);
		const auto positionY = args.at(j++);
		const auto positionZ = args.at(j++);
		const auto modelId = args.at(j++);
		const auto textureId = args.at(j++);
		auto skillVector = BuildSkillVector(args.at(j++));
		auto abilityVector = BuildAbilityVector(args.at(j++));
		const auto name = args.at(j++);
		const auto agility = args.at(j++);
		const auto strength = args.at(j++);
		const auto wisdom = args.at(j++);
		const auto intelligence = args.at(j++);
		const auto charisma = args.at(j++);
		const auto luck = args.at(j++);
		const auto endurance = args.at(j++);
		const auto health = args.at(j++);
		const auto maxHealth = args.at(j++);
		const auto mana = args.at(j++);
		const auto maxMana = args.at(j++);
		const auto stamina = args.at(j++);
		const auto maxStamina = args.at(j++);

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

		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::NpcUpdate] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const auto gameObjectId = args[j++];
		const auto posX = args[j++];
		const auto posY = args[j++];
		const auto posZ = args[j++];
		const auto movX = args[j++];
		const auto movY = args[j++];
		const auto movZ = args[j++];
		const auto agility = args[j++];
		const auto strength = args[j++];
		const auto wisdom = args[j++];
		const auto intelligence = args[j++];
		const auto charisma = args[j++];
		const auto luck = args[j++];
		const auto endurance = args[j++];
		const auto health = args[j++];
		const auto maxHealth = args[j++];
		const auto mana = args[j++];
		const auto maxMana = args[j++];
		const auto stamina = args[j++];
		const auto maxStamina = args[j++];

		std::unique_ptr<Event> e = std::make_unique<NpcUpdateEvent>
		(
			std::stol(gameObjectId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ)},
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		);

		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::PlayerUpdate] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const auto accountId = args[j++];
		const auto posX = args[j++];
		const auto posY = args[j++];
		const auto posZ = args[j++];
		const auto movX = args[j++];
		const auto movY = args[j++];
		const auto movZ = args[j++];
		const auto modelId = args[j++];
		const auto textureId = args[j++];
		const auto name = args[j++];
		const auto agility = args[j++];
		const auto strength = args[j++];
		const auto wisdom = args[j++];
		const auto intelligence = args[j++];
		const auto charisma = args[j++];
		const auto luck = args[j++];
		const auto endurance = args[j++];
		const auto health = args[j++];
		const auto maxHealth = args[j++];
		const auto mana = args[j++];
		const auto maxMana = args[j++];
		const auto stamina = args[j++];
		const auto maxStamina = args[j++];

		std::unique_ptr<Event> e = std::make_unique<PlayerUpdateEvent>
		(
			std::stol(accountId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ) },
			std::stoi(modelId), std::stoi(textureId),
			name,
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		);

		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::PropagateChatMessage] = [this](const std::vector<std::string>& args)
	{
		const auto message = args[0];
		const auto senderName = args[1];

		std::unique_ptr<Event> e = std::make_unique<PropagateChatMessageEvent>(new std::string(senderName), new std::string(message));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::ServerMessage] = [this](const std::vector<std::string>& args)
	{
		const auto message = args[0];
		const auto type = args[1];

		std::unique_ptr<Event> e = std::make_unique<ServerMessageEvent>(new std::string(message), new std::string(type));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::AttackHit] = [this](const std::vector<std::string>& args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];
		const auto damage = args[2];

		std::unique_ptr<Event> e = std::make_unique<AttackHitEvent>(std::stoi(attackerId), std::stoi(targetId), std::stoi(damage));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::AttackMiss] = [this](const std::vector<std::string>& args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];

		std::unique_ptr<Event> e = std::make_unique<AttackMissEvent>(std::stoi(attackerId), std::stoi(targetId));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::ActivateAbilitySuccess] = [this](const std::vector<std::string>& args)
	{
		const auto abilityId = args[0];

		std::unique_ptr<Event> e = std::make_unique<ActivateAbilitySuccessEvent>(std::stoi(abilityId));
		g_eventHandler.QueueEvent(e);
	};

	messageHandlers[OpCode::Pong] = [this](const std::vector<std::string>& args)
	{
		const auto pingId = args[0];

		g_game.get()->OnPong(std::stoul(pingId));
	};

	messageHandlers[OpCode::SkillIncrease] = [this](const std::vector<std::string>& args)
	{
		const auto skillId = args[0];
		const auto newValue = args[1];

		std::unique_ptr<Event> e = std::make_unique<SkillIncreaseEvent>(std::stoi(skillId), std::stoi(newValue));
		g_eventHandler.QueueEvent(e);
	};
}
