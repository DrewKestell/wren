#include "stdafx.h"
#include "SocketManager.h"
#include <OpCodes.h>
#include <GameObjectType.h>
#include "Game.h"
#include "Events/AttackHitEvent.h"
#include "Events/AttackMissEvent.h"
#include "Events/SkillIncreaseEvent.h"
#include "EventHandling/EventHandler.h"
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

constexpr auto SERVER_PORT_NUMBER = 27016;

extern EventHandler g_eventHandler;
extern std::unique_ptr<Game> g_game;

static bool logMessages{ false };

SocketManager::SocketManager()
{
	InitializeMessageHandlers();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
        throw new std::exception("Failed to initialize sockets.");

    local.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &local.sin_addr);

    to.sin_family = AF_INET;
    to.sin_port = htons(SERVER_PORT_NUMBER);
    inet_pton(AF_INET, "127.0.0.1", &to.sin_addr);

    toLen = sizeof(to);

    socketC = socket(AF_INET, SOCK_DGRAM, 0);
    int error;
    if (socketC == INVALID_SOCKET)
        error = WSAGetLastError();
    bind(socketC, (sockaddr*)&local, sizeof(local));

    DWORD nonBlocking = 1;
    ioctlsocket(socketC, FIONBIO, &nonBlocking);
}

void SocketManager::SendPacket(const OpCode opcode)
{
	std::string args[]{ "" }; // this is janky
	SendPacket(opcode, args, 0);
}

void SocketManager::SendPacket(const OpCode opCode, std::string args[], const int argCount)
{
    char buffer[PACKET_SIZE];
    memset(buffer, 0, sizeof(buffer));
	int offset{ 0 };

	memcpy(buffer, &CHECKSUM, sizeof(OpCode));
	offset += (int)sizeof(OpCode);

	memcpy(buffer + offset, &opCode, sizeof(OpCode));
	offset += (int)sizeof(OpCode);

	std::string packet{ "" };
	if (accountId != -1)
		packet += std::to_string(accountId) + "|";
	if (token != "")
		packet += token + "|";
	for (auto i = 0; i < argCount; i++)
		packet += args[i] + "|";
	strcpy_s(buffer + offset, packet.length() + 1, packet.c_str());

    auto sentBytes = sendto(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&to, sizeof(to));
    if (sentBytes != sizeof(buffer))
        throw std::exception("Failed to send packet.");
	else
	{
		// if Disconnect packet was sent successfully, update Client state
		if (opCode == OpCode::Disconnect)
		{
			accountId = -1;
			token = "";
		}
	}
}

bool SocketManager::TryRecieveMessage()
{
	char buffer[1024];
	ZeroMemory(buffer, sizeof(buffer));
	auto result = recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)& to, &toLen);
	if (result == SOCKET_ERROR)
	{
		auto errorCode = WSAGetLastError();

		if (errorCode == WSAEWOULDBLOCK)
			return false;

		throw new std::exception("WrenClient SocketManager error receiving packet. Error code: " + errorCode);
	}
	else
	{
		int offset{ 0 };

		// if the checksum is wrong, ignore the packet
		int checksum{ 0 };
		memcpy(&checksum, buffer, sizeof(OpCode));
		offset += sizeof(OpCode);
		if (checksum != (int)OpCode::Checksum)
			return true;

		OpCode opCode{ -1 };
		memcpy(&opCode, buffer + offset, sizeof(OpCode));
		offset += sizeof(OpCode);

		std::vector<std::string> args;
		auto bufferLength = strlen(buffer + offset);
		if (bufferLength > 0)
		{
			std::string arg = "";
			for (unsigned int i = offset; i < offset + bufferLength; i++)
			{
				if (buffer[i] == '|')
				{
					args.push_back(arg);
					arg = "";
				}
				else
					arg += buffer[i];
			}
		}

		const auto opCodeIndex = opCodeIndexMap[opCode];
		if (opCodeIndex >= 0 && opCodeIndex < messageHandlers.size())
		{
			messageHandlers[opCodeIndex](args);
			return true;
		}

		return false;
	}
}

bool SocketManager::Connected()
{
	return accountId != -1 && token != "";
}

void SocketManager::CloseSockets()
{
    closesocket(socketC);
    WSACleanup();
}

std::vector<std::string*>* SocketManager::BuildCharacterVector(const std::string& characterString)
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

std::vector<WrenCommon::Skill*>* SocketManager::BuildSkillVector(const std::string& skillString)
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

std::vector<Ability*>* SocketManager::BuildAbilityVector(const std::string& abilityString)
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

void SocketManager::Logout()
{
	accountId = -1;
	token = "";
}

void SocketManager::InitializeMessageHandlers()
{
	auto i = 0;

	InitializeMessageHandler(OpCode::CreateAccountFailure, i, [this](std::vector<std::string> args)
	{
		const auto error = args[0];

		g_eventHandler.QueueEvent(new CreateAccountFailedEvent{ new std::string(error)});
	});

	InitializeMessageHandler(OpCode::CreateAccountSuccess, i, [this](std::vector<std::string> args)
	{
		g_eventHandler.QueueEvent(new Event{ EventType::CreateAccountSuccess });
	});

	InitializeMessageHandler(OpCode::LoginFailure, i, [this](std::vector<std::string> args)
	{
		const auto error = args[0];
		
		g_eventHandler.QueueEvent(new LoginFailedEvent{ new std::string(error) });
	});

	InitializeMessageHandler(OpCode::LoginSuccess, i, [this](std::vector<std::string> args)
	{
		const auto accountId = args[0];
		const auto token = args[1];
		const auto characterString = args[2];
		const auto characterList = BuildCharacterVector(characterString);

		this->accountId = std::stoi(accountId);
		this->token = std::string(token);

		g_eventHandler.QueueEvent(new LoginSuccessEvent{ characterList });
	});

	InitializeMessageHandler(OpCode::CreateCharacterFailure, i, [this](std::vector<std::string> args)
	{
		const auto error = args[0];

		g_eventHandler.QueueEvent(new CreateCharacterFailedEvent{ new std::string(error) });
	});

	InitializeMessageHandler(OpCode::CreateCharacterSuccess, i, [this](std::vector<std::string> args)
	{
		const auto characterString = args[0];
		const auto characterList = BuildCharacterVector(characterString);

		g_eventHandler.QueueEvent(new CreateCharacterSuccessEvent{ characterList });
	});

	InitializeMessageHandler(OpCode::EnterWorldSuccess, i, [this](std::vector<std::string> args)
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
			new std::string(name),
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		};
		g_eventHandler.QueueEvent(enterWorldSuccessEvent);
	});

	InitializeMessageHandler(OpCode::NpcUpdate, i, [this](std::vector<std::string> args)
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
	});

	InitializeMessageHandler(OpCode::PlayerUpdate, i, [this](std::vector<std::string> args)
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
			new std::string(name),
			std::stoi(agility), std::stoi(strength), std::stoi(wisdom), std::stoi(intelligence), std::stoi(charisma), std::stoi(luck), std::stoi(endurance),
			std::stoi(health), std::stoi(maxHealth), std::stoi(mana), std::stoi(maxMana), std::stoi(stamina), std::stoi(maxStamina)
		};
		g_eventHandler.QueueEvent(playerUpdateEvent);
	});

	InitializeMessageHandler(OpCode::PropagateChatMessage, i, [this](std::vector<std::string> args)
	{
		const auto message = args[0];
		const auto senderName = args[1];

		g_eventHandler.QueueEvent(new PropagateChatMessageEvent(new std::string(senderName), new std::string(message)));
	});

	InitializeMessageHandler(OpCode::ServerMessage, i, [this](std::vector<std::string> args)
	{
		const auto message = args[0];
		const auto type = args[1];

		g_eventHandler.QueueEvent(new ServerMessageEvent(new std::string(message), new std::string(type)));
	});

	InitializeMessageHandler(OpCode::AttackHit, i, [this](std::vector<std::string> args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];
		const auto damage = args[2];

		g_eventHandler.QueueEvent(new AttackHitEvent(std::stoi(attackerId), std::stoi(targetId), std::stoi(damage)));
	});

	InitializeMessageHandler(OpCode::CreateAccountFailure, i, [this](std::vector<std::string> args)
	{
		const auto attackerId = args[0];
		const auto targetId = args[1];

		g_eventHandler.QueueEvent(new AttackMissEvent(std::stoi(attackerId), std::stoi(targetId)));
	});

	InitializeMessageHandler(OpCode::ActivateAbilitySuccess, i, [this](std::vector<std::string> args)
	{
		const auto abilityId = args[0];

		g_eventHandler.QueueEvent(new ActivateAbilitySuccessEvent(std::stoi(abilityId)));
	});

	InitializeMessageHandler(OpCode::Pong, i, [this](std::vector<std::string> args)
	{
		const auto pingId = args[0];

		g_game.get()->OnPong(std::stoul(pingId));
	});

	InitializeMessageHandler(OpCode::SkillIncrease, i, [this](std::vector<std::string> args)
	{
		const auto skillId = args[0];
		const auto newValue = args[1];

		g_eventHandler.QueueEvent(new SkillIncreaseEvent(std::stoi(skillId), std::stoi(newValue)));
	});

	InitializeMessageHandler(OpCode::CreateAccountFailure, i, [this](std::vector<std::string> args)
	{
		
	});

	InitializeMessageHandler(OpCode::CreateAccountFailure, i, [this](std::vector<std::string> args)
	{
		
	});

	InitializeMessageHandler(OpCode::CreateAccountFailure, i, [this](std::vector<std::string> args)
	{
		
	});
}

void SocketManager::InitializeMessageHandler(const OpCode opCode, int& index, std::function<void(std::vector<std::string> args)> function)
{
	opCodeIndexMap[opCode] = index++;
	messageHandlers.push_back(function);
}