#include "stdafx.h"
#include "SocketManager.h"
#include <OpCodes.h>
#include <GameObjectType.h>
#include "Game.h"
#include "Events/AttackHitEvent.h"
#include "Events/AttackMissEvent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/EnterWorldSuccessEvent.h"
#include "EventHandling/Events/GameObjectUpdateEvent.h"
#include "EventHandling/Events/OtherPlayerUpdateEvent.h"
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

void SocketManager::SendPacket(const std::string& opcode)
{
	std::string args[]{ "" }; // this is janky
	SendPacket(opcode, args, 0);
}

void SocketManager::SendPacket(const std::string& opcode, std::string args[], const int argCount)
{
    std::string packet = std::string(CHECKSUM) + opcode;

	if (accountId != "")
		packet += accountId + "|";
	if (token != "")
		packet += token + "|";

	for (auto i = 0; i < argCount; i++)
		packet += args[i] + "|";

    char buffer[1024];
    ZeroMemory(buffer, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), packet.c_str());
    auto sentBytes = sendto(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&to, sizeof(sockaddr_in));
    if (sentBytes != sizeof(buffer))
        throw std::exception("Failed to send packet.");
	else
	{
		if (opcode == std::string(OPCODE_DISCONNECT))
		{
			accountId = "";
			token = "";
		}
	}
}

bool SocketManager::Connected()
{
	return accountId != "" && token != "";
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
        if (!MessagePartsEqual(checksumArr, CHECKSUM.c_str(), checksumArrLen))
        {
			/*if (logMessages)
				std::cout << "Wrong checksum. Ignoring packet.\n";*/
			g_eventHandler.QueueEvent(new Event{ EventType::WrongChecksum });
			return true;
        }

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

			/*if (logMessages)
			{
				std::cout << "Args:\n";
				for_each(args.begin(), args.end(), [](std::string* str) { std::cout << "  " << *str << "\n";  });
				std::cout << "\n";
			}*/
        }

        if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
			if (logMessages)
				std::cout << "Failed to create account. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new CreateAccountFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_SUCCESSFUL, opcodeArrLen))
        {
			if (logMessages)
				std::cout << "Successfully created account.\n";
			g_eventHandler.QueueEvent(new Event{EventType::CreateAccountSuccess});
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_SUCCESSFUL, opcodeArrLen))
        {
			const auto accountId = args[0];
            const auto token = args[1];
            const auto characterString = args[2];
            const auto characterList = BuildCharacterVector(characterString);

			this->accountId = *accountId;
			this->token = *token;

			if (logMessages)
				std::cout << "Login successful. Token received: " + *token + "\n";
			g_eventHandler.QueueEvent(new LoginSuccessEvent{ characterList });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
			if (logMessages)
				std::cout << "Login failed. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new LoginFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_SUCCESSFUL, opcodeArrLen))
        {
            const auto characterString = args[0];
            const auto characterList = BuildCharacterVector(characterString);

			if (logMessages)
				std::cout << "Character creation successful.";
			g_eventHandler.QueueEvent(new CreateCharacterSuccessEvent{ characterList });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
			if (logMessages)
				std::cout << "Character creation failed. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new CreateCharacterFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_ENTER_WORLD_SUCCESSFUL, opcodeArrLen))
        {
			const auto accountId = args[0];
			const auto positionX = args[1];
			const auto positionY = args[2];
			const auto positionZ = args[3];
			const auto modelId = args[4];
			const auto textureId = args[5];
			const auto skillString = args[6];
			const auto abilityString = args[7];
			const auto name = args[8];

			if (logMessages)
				std::cout << "Connected to game world!\n";
			g_eventHandler.QueueEvent(new EnterWorldSuccessEvent(std::stoi(*accountId), XMFLOAT3{ std::stof(*positionX), std::stof(*positionY), std::stof(*positionZ) }, std::stoi(*modelId), std::stoi(*textureId), BuildSkillVector(*skillString), BuildAbilityVector(*abilityString), name));
			return true;
        }
		else if (MessagePartsEqual(opcodeArr, OPCODE_DELETE_CHARACTER_SUCCESSFUL, opcodeArrLen))
		{
			const auto characterString = args[0];
			const auto characterList = BuildCharacterVector(characterString);

			if (logMessages)
				std::cout << "Delete character successful.";
			g_eventHandler.QueueEvent(new DeleteCharacterSuccessEvent{ characterList });
			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_GAMEOBJECT_UPDATE, opcodeArrLen))
		{
			const auto gameObjectId = args[0];
			const auto posX = args[1];
			const auto posY = args[2];
			const auto posZ = args[3];
			const auto movX = args[4];
			const auto movY = args[5];
			const auto movZ = args[6];
			const auto type = args[7];

			g_eventHandler.QueueEvent(new GameObjectUpdateEvent{ std::stol(*gameObjectId), std::stof(*posX), std::stof(*posY), std::stof(*posZ), std::stof(*movX), std::stof(*movY), std::stof(*movZ), static_cast<GameObjectType>(std::stoi(*type)) });
			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_OTHER_PLAYER_UPDATE, opcodeArrLen))
		{
			const auto accountId = args[0];
			const auto posX = args[1];
			const auto posY = args[2];
			const auto posZ = args[3];
			const auto movX = args[4];
			const auto movY = args[5];
			const auto movZ = args[6];
			const auto modelId = args[7];
			const auto textureId = args[8];
			const auto name = args[9];

			g_eventHandler.QueueEvent(new OtherPlayerUpdateEvent{ std::stol(*accountId), std::stof(*posX), std::stof(*posY), std::stof(*posZ), std::stof(*movX), std::stof(*movY), std::stof(*movZ), std::stoi(*modelId), std::stoi(*textureId), name });
			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_PROPAGATE_CHAT_MESSAGE, opcodeArrLen))
		{
			const auto message = args[0];
			const auto senderName = args[1];

			g_eventHandler.QueueEvent(new PropagateChatMessage(senderName, message));

			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_SERVER_MESSAGE, opcodeArrLen))
		{
			const auto message = args[0];
			const auto type = args[1];

			g_eventHandler.QueueEvent(new ServerMessageEvent(message, type));

			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_ACTIVATE_ABILITY_SUCCESS, opcodeArrLen))
		{
			const auto abilityId = args[0];

			g_eventHandler.QueueEvent(new ActivateAbilitySuccessEvent(std::stoi(*abilityId)));

			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_ATTACK_HIT, opcodeArrLen))
		{
			const auto attackerId = args[0];
			const auto targetId = args[1];
			const auto damage = args[2];

			g_eventHandler.QueueEvent(new AttackHitEvent(std::stoi(*attackerId), std::stoi(*targetId), std::stoi(*damage)));

			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_ATTACK_MISS, opcodeArrLen))
		{
			const auto attackerId = args[0];
			const auto targetId = args[1];

			g_eventHandler.QueueEvent(new AttackMissEvent(std::stoi(*attackerId), std::stoi(*targetId)));

			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_PONG, opcodeArrLen))
		{
			const auto pingId = args[0];

			g_game.get()->OnPong(std::stoul(*pingId));

			return true;
		}
		else
		{
			if (logMessages)
				std::cout << "Opcode not implemented.\n";
			g_eventHandler.QueueEvent(new Event{ EventType::OpcodeNotImplemented });
			return true;
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

std::vector<Skill*>* SocketManager::BuildSkillVector(std::string& skillString)
{
	std::vector<Skill*>* skillList = new std::vector<Skill*>;
	char param = 0;
	std::string skillId = "";
	std::string name = "";
	std::string value = "";
	for (auto i = 0; i < skillString.length(); i++)
	{
		if (skillString[i] == ';')
		{
			skillList->push_back(new Skill(std::stoi(skillId), name, std::stoi(value)));
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
	accountId = "";
	token = "";
}