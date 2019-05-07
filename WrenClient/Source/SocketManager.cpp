#include "stdafx.h"
#include "SocketManager.h"
#include <OpCodes.h>
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/EnterWorldSuccessEvent.h"
#include "EventHandling/Events/GameObjectUpdateEvent.h"

constexpr auto SERVER_PORT_NUMBER = 27016;

extern EventHandler g_eventHandler;

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

bool SocketManager::MessagePartsEqual(const char* first, const char* second, int length)
{
    for (auto i = 0; i < length; i++)
    {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

void SocketManager::SendPacket(const std::string& opcode, const int argCount, ...)
{
    std::string packet = std::string(CHECKSUM) + opcode;

	if (m_token != "")
		packet += m_token + "|";

    va_list args;
    va_start(args, argCount);

    for (auto i = 0; i < argCount; i++)
        packet += (va_arg(args, std::string) + "|");
    va_end(args);

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
			m_token = "";
		}
	}
}

bool SocketManager::Connected()
{
	return m_token != "";
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
        printf("Received message from server.\n");

        const auto checksumArrLen = 8;
        char checksumArr[checksumArrLen];
        memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));
        if (!MessagePartsEqual(checksumArr, CHECKSUM.c_str(), checksumArrLen))
        {
            std::cout << "Wrong checksum. Ignoring packet.\n";
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

            std::cout << "Args:\n";
            for_each(args.begin(), args.end(), [](std::string* str) { std::cout << "  " << *str << "\n";  });
            std::cout << "\n";
        }

        if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Failed to create account. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new CreateAccountFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_SUCCESSFUL, opcodeArrLen))
        {
            std::cout << "Successfully created account.\n";
			g_eventHandler.QueueEvent(new Event{EventType::CreateAccountSuccess});
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_SUCCESSFUL, opcodeArrLen))
        {
            const auto token = args[0];
            const auto characterString = args[1];
            const auto characterList = BuildCharacterVector(characterString);

			m_token = *token;

            std::cout << "Login successful. Token received: " + *token + "\n";
			g_eventHandler.QueueEvent(new LoginSuccessEvent{ characterList });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Login failed. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new LoginFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_SUCCESSFUL, opcodeArrLen))
        {
            const auto characterString = args[0];
            const auto characterList = BuildCharacterVector(characterString);

            std::cout << "Character creation successful.";
			g_eventHandler.QueueEvent(new CreateCharacterSuccessEvent{ characterList });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Character creation failed. Reason: " + *error + "\n";
			g_eventHandler.QueueEvent(new CreateCharacterFailedEvent{ error });
			return true;
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_ENTER_WORLD_SUCCESSFUL, opcodeArrLen))
        {
			const auto id = args[0];
			const auto positionX = args[1];
			const auto positionY = args[2];
			const auto positionZ = args[3];

            std::cout << "Connected to game world!\n";
			g_eventHandler.QueueEvent(new EnterWorldSuccessEvent(std::stoi(*id), XMFLOAT3{ std::stof(*positionX), std::stof(*positionY), std::stof(*positionZ) }));
			return true;
        }
		else if (MessagePartsEqual(opcodeArr, OPCODE_DELETE_CHARACTER_SUCCESSFUL, opcodeArrLen))
		{
			const auto characterString = args[0];
			const auto characterList = BuildCharacterVector(characterString);

			std::cout << "Delete character successful.";
			g_eventHandler.QueueEvent(new DeleteCharacterSuccessEvent{ characterList });
			return true;
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_PLAYER_CORRECTION, opcodeArrLen))
		{
			const auto idCounter = args[0];
			const auto posX = args[1];
			const auto posY = args[2];
			const auto posZ = args[3];
			const auto state = args[4];
			const auto direction = args[5];

			// publish event
		}
		else if (MessagePartsEqual(opcodeArr, OPCODE_GAMEOBJECT_UPDATE, opcodeArrLen))
		{
			const auto characterId = args[0];
			const auto posX = args[1];
			const auto posY = args[2];
			const auto posZ = args[3];
			const auto movX = args[4];
			const auto movY = args[5];
			const auto movZ = args[6];

			g_eventHandler.QueueEvent(new GameObjectUpdateEvent{ std::stol(*characterId), std::stof(*posX), std::stof(*posY), std::stof(*posZ), std::stof(*movX), std::stof(*movY), std::stof(*movZ) });
			return true;
		}
		else
		{
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