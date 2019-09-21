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

bool ClientSocketManager::Connected()
{
	return accountId != -1 && token != "";
}

std::vector<std::string*>* ClientSocketManager::BuildCharacterVector(const std::string& characterString)
{
    std::vector<std::string*>* characterList = new std::vector<std::string*>;
	auto arg = new std::string;
    for (auto i = 0; i < characterString.length(); i++)
    {
        if (characterString[i] == ';')
        {
            characterList->push_back(arg);
			arg = new std::string;
        }
        else
            *arg += characterString[i];
    }
    return characterList;
}

std::vector<WrenCommon::Skill*>* ClientSocketManager::BuildSkillVector(const std::string& skillString)
{
	std::vector<WrenCommon::Skill*>* skillList = new std::vector<WrenCommon::Skill*>;
	char param = 0;
	std::string skillId = "";
	std::string name = "";
	std::string value = "";
	for (auto i = 0; i < skillString.length(); i++)
	{
		if (skillString[i] == ';')
		{
			skillList->push_back(new WrenCommon::Skill(std::stoi(skillId), name, std::stoi(value)));
			param = 0;
			skillId = "";
			name = "";
			value = "";
		}
		else if (skillString[i] == '%')
				param++;
		else
		{
			if (param == 0)
				skillId += skillString[i];
			else if (param == 1)
				name += skillString[i];
			else
				value += skillString[i];
		}
	}
	return skillList;
}

std::vector<Ability*>* ClientSocketManager::BuildAbilityVector(const std::string& abilityString)
{
	std::vector<Ability*>* abilityList = new std::vector<Ability*>;
	char param = 0;
	std::string abilityId = "";
	std::string name = "";
	std::string spriteId = "";
	std::string toggled = "";
	std::string targeted = "";
	for (auto i = 0; i < abilityString.length(); i++)
	{
		if (abilityString[i] == ';')
		{
			abilityList->push_back(new Ability(std::stoi(abilityId), name, std::stoi(spriteId), toggled == "1", targeted == "1"));
			param = 0;
			abilityId = "";
			name = "";
			spriteId = "";
			toggled = "";
			targeted = "";
		}
		else if (abilityString[i] == '%')
			param++;
		else
		{
			if (param == 0)
				abilityId += abilityString[i];
			else if (param == 1)
				name += abilityString[i];
			else if (param == 2)
				spriteId += abilityString[i];
			else if (param == 3)
				toggled += abilityString[i];
			else
				targeted += abilityString[i];
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

		g_eventHandler.QueueEvent(new CreateAccountFailedEvent{ new std::string(error) });
	};

	messageHandlers[OpCode::CreateAccountSuccess] = [this](const std::vector<std::string>& args)
	{
		g_eventHandler.QueueEvent(new Event{ EventType::CreateAccountSuccess });
	};

	messageHandlers[OpCode::LoginFailure] = [this](const std::vector<std::string>& args)
	{
		const auto error = args[0];
		
		g_eventHandler.QueueEvent(new LoginFailedEvent{ new std::string(error) });
	};

	messageHandlers[OpCode::LoginSuccess] = [this](const std::vector<std::string>& args)
	{
		const auto accountId = args[0];
		const auto token = args[1];
		const auto characterString = args[2];
		const auto characterList = BuildCharacterVector(characterString);

		this->accountId = std::stoi(accountId);
		this->token = std::string(token);

		g_eventHandler.QueueEvent(new LoginSuccessEvent{ characterList });
	};

	messageHandlers[OpCode::CreateCharacterFailure] = [this](const std::vector<std::string>& args)
	{
		const auto error = args[0];

		g_eventHandler.QueueEvent(new CreateCharacterFailedEvent{ new std::string(error) });
	};

	messageHandlers[OpCode::CreateCharacterSuccess] = [this](const std::vector<std::string>& args)
	{
		const auto characterString = args[0];
		const auto characterList = BuildCharacterVector(characterString);

		g_eventHandler.QueueEvent(new CreateCharacterSuccessEvent{ characterList });
	};

	messageHandlers[OpCode::EnterWorldSuccess] = [this](const std::vector<std::string>& args)
	{
		auto j = 0;
		const auto accountId = args[j++];
		const auto positionX = args[j++];
		const auto positionY = args[j++];
		const auto positionZ = args[j++];
		const auto modelId = args[j++];
		const auto textureId = args[j++];
		const auto skillString = args[j++];
		const auto abilityString = args[j++];
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

		const auto enterWorldSuccessEvent = new EnterWorldSuccessEvent
		{
			std::stoi(accountId),
			XMFLOAT3{ std::stof(positionX), std::stof(positionY), std::stof(positionZ) },
			std::stoi(modelId), std::stoi(textureId),
			BuildSkillVector(skillString), BuildAbilityVector(abilityString),
			name,
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		};
		g_eventHandler.QueueEvent(enterWorldSuccessEvent);
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

		const auto gameObjectUpdateEvent = new NpcUpdateEvent
		{
			std::stol(gameObjectId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ)},
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		};
		g_eventHandler.QueueEvent(gameObjectUpdateEvent);
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

		const auto playerUpdateEvent = new PlayerUpdateEvent
		{
			std::stol(accountId),
			XMFLOAT3{ std::stof(posX), std::stof(posY), std::stof(posZ)}, XMFLOAT3{ std::stof(movX), std::stof(movY), std::stof(movZ) },
			std::stoi(modelId), std::stoi(textureId),
			name,
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		};
		g_eventHandler.QueueEvent(playerUpdateEvent);
	};

	messageHandlers[OpCode::PropagateChatMessage] = [this](const std::vector<std::string>& args)
	{
		const auto message = args[0];
		const auto senderName = args[1];

		g_eventHandler.QueueEvent(new PropagateChatMessageEvent(new std::string(senderName), new std::string(message)));
	};

	messageHandlers[OpCode::ServerMessage] = [this](const std::vector<std::string>& args)
	{
		const auto message = args[0];
		const auto type = args[1];

		g_eventHandler.QueueEvent(new ServerMessageEvent(new std::string(message), new std::string(type)));
	};

	messageHandlers[OpCode::AttackHit] = [this](const std::vector<std::string>& args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];
		const auto damage = args[2];

		g_eventHandler.QueueEvent(new AttackHitEvent(std::stoi(attackerId), std::stoi(targetId), std::stoi(damage)));
	};

	messageHandlers[OpCode::CreateAccountFailure] = [this](const std::vector<std::string>& args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];

		g_eventHandler.QueueEvent(new AttackMissEvent(std::stoi(attackerId), std::stoi(targetId)));
	};

	messageHandlers[OpCode::ActivateAbilitySuccess] = [this](const std::vector<std::string>& args)
	{
		const auto abilityId = args[0];

		g_eventHandler.QueueEvent(new ActivateAbilitySuccessEvent(std::stoi(abilityId)));
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

		g_eventHandler.QueueEvent(new SkillIncreaseEvent(std::stoi(skillId), std::stoi(newValue)));
	};
}
