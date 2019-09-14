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
#include "EventHandling/Events/PropagateChatMessage.h"
#include "EventHandling/Events/ServerMessageEvent.h"
#include "EventHandling/Events/ActivateAbilitySuccessEvent.h"

constexpr auto SERVER_PORT_NUMBER = 27016;

extern EventHandler g_eventHandler;
extern std::unique_ptr<Game> g_game;

static bool logMessages{ false };

SocketManager::SocketManager()
{
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

bool SocketManager::MessagePartsEqual(const char* first, const char* second, int length) const
{
    for (auto i = 0; i < length; i++)
    {
        if (first[i] != second[i])
            return false;
    }
    return true;
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

bool SocketManager::Connected()
{
	return accountId != -1 && token != "";
}

void SocketManager::CloseSockets()
{
    closesocket(socketC);
    WSACleanup();
}

bool SocketManager::TryRecieveMessage()
{
    char buffer[1024];
    char str[INET_ADDRSTRLEN];
    ZeroMemory(buffer, sizeof(buffer));
    ZeroMemory(str, sizeof(str));
    auto result = recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&to, &toLen);
    int errorCode;
    if (result == -1)
        errorCode = WSAGetLastError();
    if (result != SOCKET_ERROR)
    {
		if (logMessages)
			printf("Received message from server.\n");

        const auto checksumArrLen = 8;
        char checksumArr[checksumArrLen];
        memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));
   //     if (!MessagePartsEqual(checksumArr, CHECKSUM.c_str(), checksumArrLen))
   //     {
			///*if (logMessages)
			//	std::cout << "Wrong checksum. Ignoring packet.\n";*/
			//g_eventHandler.QueueEvent(new Event{ EventType::WrongChecksum });
			//return true;
   //     }

        const auto opcodeArrLen = 2;
        char opcodeArr[opcodeArrLen];
        memcpy(&opcodeArr[0], &buffer[8], opcodeArrLen * sizeof(char));

        std::vector<std::string*> args;
        auto bufferLength = strlen(buffer);
        if (bufferLength > 10)
        {
			auto arg = new std::string;
            for (unsigned int i = 10; i < bufferLength; i++)
            {
				
                if (buffer[i] == '|')
                {
                    args.push_back(arg);
					arg = new std::string;
                }
                else
                    *arg += buffer[i];
            }
        }

		OpCode opCode = OpCode::ActivateAbility; // TODO
		switch (opCode)
		{
			case OpCode::CreateAccountFailure:
			{
				const auto error = args[0];
				if (logMessages)
					std::cout << "Failed to create account. Reason: " + *error + "\n";
				g_eventHandler.QueueEvent(new CreateAccountFailedEvent{ error });
				return true;
			}
			case OpCode::CreateAccountSuccess:
			{
				if (logMessages)
					std::cout << "Successfully created account.\n";
				g_eventHandler.QueueEvent(new Event{ EventType::CreateAccountSuccess });
				return true;
			}
			case OpCode::LoginSuccess:
			{
				const auto accountId = args[0];
				const auto token = args[1];
				const auto characterString = args[2];
				const auto characterList = BuildCharacterVector(characterString);

				this->accountId = std::stoi(*accountId);
				this->token = *token;

				if (logMessages)
					std::cout << "Login successful. Token received: " + *token + "\n";
				g_eventHandler.QueueEvent(new LoginSuccessEvent{ characterList });
				return true;
			}
			case OpCode::LoginFailure:
			{
				const auto error = args[0];
				if (logMessages)
					std::cout << "Login failed. Reason: " + *error + "\n";
				g_eventHandler.QueueEvent(new LoginFailedEvent{ error });
				return true;
			}
			case OpCode::CreateCharacterSuccess:
			{
				const auto characterString = args[0];
				const auto characterList = BuildCharacterVector(characterString);

				if (logMessages)
					std::cout << "Character creation successful.";
				g_eventHandler.QueueEvent(new CreateCharacterSuccessEvent{ characterList });
				return true;
			}
			case OpCode::CreateCharacterFailure:
			{
				const auto error = args[0];
				if (logMessages)
					std::cout << "Character creation failed. Reason: " + *error + "\n";
				g_eventHandler.QueueEvent(new CreateCharacterFailedEvent{ error });
				return true;
			}
			case OpCode::EnterWorldSuccess:
			{
				auto i = 0;
				const auto accountId = args[i++];
				const auto positionX = args[i++];
				const auto positionY = args[i++];
				const auto positionZ = args[i++];
				const auto modelId = args[i++];
				const auto textureId = args[i++];
				const auto skillString = args[i++];
				const auto abilityString = args[i++];
				const auto name = args[i++];
				const auto agility = args[i++];
				const auto strength = args[i++];
				const auto wisdom = args[i++];
				const auto intelligence = args[i++];
				const auto charisma = args[i++];
				const auto luck = args[i++];
				const auto endurance = args[i++];
				const auto health = args[i++];
				const auto maxHealth = args[i++];
				const auto mana = args[i++];
				const auto maxMana = args[i++];
				const auto stamina = args[i++];
				const auto maxStamina = args[i++];

				const auto enterWorldSuccessEvent = new EnterWorldSuccessEvent
				{
					std::stoi(*accountId),
					XMFLOAT3{ std::stof(*positionX), std::stof(*positionY), std::stof(*positionZ) },
					std::stoi(*modelId), std::stoi(*textureId),
					BuildSkillVector(*skillString), BuildAbilityVector(*abilityString),
					name,
					std::stoi(*agility), std::stoi(*strength), std::stoi(*wisdom), std::stoi(*intelligence), std::stoi(*charisma), std::stoi(*luck), std::stoi(*endurance),
					std::stoi(*health), std::stoi(*maxHealth), std::stoi(*mana), std::stoi(*maxMana), std::stoi(*stamina), std::stoi(*maxStamina)
				};
				g_eventHandler.QueueEvent(enterWorldSuccessEvent);
				return true;
			}
			case OpCode::DeleteCharacterSuccess:
			{
				const auto characterString = args[0];
				const auto characterList = BuildCharacterVector(characterString);

				if (logMessages)
					std::cout << "Delete character successful.";
				g_eventHandler.QueueEvent(new DeleteCharacterSuccessEvent{ characterList });
				return true;
			}
			case OpCode::NpcUpdate:
			{
				auto i = 0;
				const auto gameObjectId = args[i++];
				const auto posX = args[i++];
				const auto posY = args[i++];
				const auto posZ = args[i++];
				const auto movX = args[i++];
				const auto movY = args[i++];
				const auto movZ = args[i++];
				const auto agility = args[i++];
				const auto strength = args[i++];
				const auto wisdom = args[i++];
				const auto intelligence = args[i++];
				const auto charisma = args[i++];
				const auto luck = args[i++];
				const auto endurance = args[i++];
				const auto health = args[i++];
				const auto maxHealth = args[i++];
				const auto mana = args[i++];
				const auto maxMana = args[i++];
				const auto stamina = args[i++];
				const auto maxStamina = args[i++];

				const auto gameObjectUpdateEvent = new NpcUpdateEvent
				{
					std::stol(*gameObjectId),
					XMFLOAT3{ std::stof(*posX), std::stof(*posY), std::stof(*posZ)}, XMFLOAT3{ std::stof(*movX), std::stof(*movY), std::stof(*movZ)},
					std::stoi(*agility), std::stoi(*strength), std::stoi(*wisdom), std::stoi(*intelligence), std::stoi(*charisma), std::stoi(*luck), std::stoi(*endurance),
					std::stoi(*health), std::stoi(*maxHealth), std::stoi(*mana), std::stoi(*maxMana), std::stoi(*stamina), std::stoi(*maxStamina)
				};
				g_eventHandler.QueueEvent(gameObjectUpdateEvent);
				return true;
			}
			case OpCode::PlayerUpdate:
			{
				auto i = 0;
				const auto accountId = args[i++];
				const auto posX = args[i++];
				const auto posY = args[i++];
				const auto posZ = args[i++];
				const auto movX = args[i++];
				const auto movY = args[i++];
				const auto movZ = args[i++];
				const auto modelId = args[i++];
				const auto textureId = args[i++];
				const auto name = args[i++];
				const auto agility = args[i++];
				const auto strength = args[i++];
				const auto wisdom = args[i++];
				const auto intelligence = args[i++];
				const auto charisma = args[i++];
				const auto luck = args[i++];
				const auto endurance = args[i++];
				const auto health = args[i++];
				const auto maxHealth = args[i++];
				const auto mana = args[i++];
				const auto maxMana = args[i++];
				const auto stamina = args[i++];
				const auto maxStamina = args[i++];

				const auto playerUpdateEvent = new PlayerUpdateEvent
				{
					std::stol(*accountId),
					XMFLOAT3{ std::stof(*posX), std::stof(*posY), std::stof(*posZ)}, XMFLOAT3{ std::stof(*movX), std::stof(*movY), std::stof(*movZ) },
					std::stoi(*modelId), std::stoi(*textureId),
					name,
					std::stoi(*agility), std::stoi(*strength), std::stoi(*wisdom), std::stoi(*intelligence), std::stoi(*charisma), std::stoi(*luck), std::stoi(*endurance),
					std::stoi(*health), std::stoi(*maxHealth), std::stoi(*mana), std::stoi(*maxMana), std::stoi(*stamina), std::stoi(*maxStamina)
				};
				g_eventHandler.QueueEvent(playerUpdateEvent);
				return true;
			}
			case OpCode::PropagateChatMessage:
			{
				const auto message = args[0];
				const auto senderName = args[1];

				g_eventHandler.QueueEvent(new PropagateChatMessage(senderName, message));

				return true;
			}
			case OpCode::ServerMessage:
			{
				const auto message = args[0];
				const auto type = args[1];

				g_eventHandler.QueueEvent(new ServerMessageEvent(message, type));

				return true;
			}
			case OpCode::ActivateAbilitySuccess:
			{
				const auto abilityId = args[0];

				g_eventHandler.QueueEvent(new ActivateAbilitySuccessEvent(std::stoi(*abilityId)));

				return true;
			}
			case OpCode::AttackHit:
			{
				const auto attackerId = args[0];
				const auto targetId = args[1];
				const auto damage = args[2];

				g_eventHandler.QueueEvent(new AttackHitEvent(std::stoi(*attackerId), std::stoi(*targetId), std::stoi(*damage)));

				return true;
			}
			case OpCode::AttackMiss:
			{
				const auto attackerId = args[0];
				const auto targetId = args[1];

				g_eventHandler.QueueEvent(new AttackMissEvent(std::stoi(*attackerId), std::stoi(*targetId)));

				return true;
			}
			case OpCode::Pong:
			{
				const auto pingId = args[0];

				g_game.get()->OnPong(std::stoul(*pingId));

				return true;
			}
			case OpCode::SkillIncrease:
			{
				const auto skillId = args[0];
				const auto newValue = args[1];

				g_eventHandler.QueueEvent(new SkillIncreaseEvent(std::stoi(*skillId), std::stoi(*newValue)));

				return true;
			}
			default:
			{
				if (logMessages)
					std::cout << "Opcode not implemented.\n";
				g_eventHandler.QueueEvent(new Event{ EventType::OpcodeNotImplemented });
				return true;
			}
		}
    }
	return false;
}

std::vector<std::string*>* SocketManager::BuildCharacterVector(std::string* characterString)
{
    std::vector<std::string*>* characterList = new std::vector<std::string*>;
	auto arg = new std::string;
    for (auto i = 0; i < characterString->length(); i++)
    {
        if ((*characterString)[i] == ';')
        {
            characterList->push_back(arg);
			arg = new std::string;
        }
        else
            *arg += (*characterString)[i];
    }
    return characterList;
}

std::vector<WrenCommon::Skill*>* SocketManager::BuildSkillVector(std::string& skillString)
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

std::vector<Ability*>* SocketManager::BuildAbilityVector(std::string& abilityString)
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