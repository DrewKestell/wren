#include <winsock2.h>
#include <Ws2tcpip.h>
#include <sodium.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include "SocketManager.h"
#include "Repository.h"
#include "Player.h"

constexpr auto PLAYER_NOT_FOUND = "Player not found.";
constexpr auto SOCKET_INIT_FAILED = "Failed to initialize sockets.";
constexpr auto INCORRECT_USERNAME = "Incorrect Username.";
constexpr auto INCORRECT_PASSWORD = "Incorrect Password.";
constexpr auto ACCOUNT_ALREADY_EXISTS = "Account already exists.";
constexpr auto LIBSODIUM_MEMORY_ERROR = "Ran out of memory while hashing password.";
constexpr auto CHARACTER_ALREADY_EXISTS = "Character already exists.";

constexpr auto TIMEOUT_DURATION = 30000; // 30000ms == 30s
constexpr auto PORT_NUMBER = 27015;

// CONSTRUCTOR
SocketManager::SocketManager(Repository& repository) : repository(repository)
{
    sodium_init();

    local.sin_family = AF_INET;
    local.sin_port = htons(PORT_NUMBER);
    local.sin_addr.s_addr = INADDR_ANY;

    fromlen = sizeof(from);

    socketS = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketS, (sockaddr*)&local, sizeof(local));

    DWORD nonBlocking = 1;
    ioctlsocket(socketS, FIONBIO, &nonBlocking);
}

// PRIVATE
std::vector<Player*>::iterator SocketManager::GetPlayer(const std::string& token)
{
	const auto it = find_if(players.begin(), players.end(), [&token](Player& player) { return player.GetToken() == token; });
	if (it == players.end())
		throw std::exception(PLAYER_NOT_FOUND);
	return it;
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

void SocketManager::Login(
	const std::string& accountName,
	const std::string& password,
	const std::string& ipAndPort)
{
	std::string error;
	auto account = repository.GetAccount(accountName);
	if (account)
	{
		auto passwordArr = password.c_str();
		if (crypto_pwhash_str_verify(account->GetPassword().c_str(), passwordArr, strlen(passwordArr)) != 0)
			error = INCORRECT_PASSWORD;
		else
		{
			GUID guid;
			CoCreateGuid(&guid);
			char guid_cstr[39];
			snprintf(guid_cstr, sizeof(guid_cstr),
				"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
			const std::string token = std::string(guid_cstr);

			auto player = new Player
			{
				account->GetId(),
				token,
				ipAndPort,
				GetTickCount()
			};
			players.push_back(player);

			SendPacket(OPCODE_LOGIN_SUCCESSFUL, 1, token);
			std::cout << "AccountId " << account->GetId() << " connected to the server.\n";
		}

	}
	else
		error = INCORRECT_USERNAME;

	if (error != "")
		SendPacket(OPCODE_LOGIN_UNSUCCESSFUL, 1, error);
}

void SocketManager::Logout(const std::string& token)
{
	const auto it = GetPlayer(token);
	delete(*it);
	players.erase(it);
}

void SocketManager::CreateAccount(const std::string& accountName, const std::string& password)
{
	if (repository.AccountExists(accountName))
	{
		const std::string error = ACCOUNT_ALREADY_EXISTS;
		SendPacket(OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL, 1, error);
	}
	else
	{
		char hashedPassword[crypto_pwhash_STRBYTES];
		auto passwordArr = password.c_str();
		const auto result = crypto_pwhash_str(
			hashedPassword,
			passwordArr,
			strlen(passwordArr),
			crypto_pwhash_OPSLIMIT_INTERACTIVE,
			crypto_pwhash_MEMLIMIT_INTERACTIVE);
		if (result != 0)
			throw std::exception(LIBSODIUM_MEMORY_ERROR);

		repository.CreateAccount(accountName, hashedPassword);
		SendPacket(OPCODE_CREATE_ACCOUNT_SUCCESSFUL);
	}
}

void SocketManager::CreateCharacter(const std::string& token, const std::string& characterName)
{
	const auto it = GetPlayer(token);
	if (repository.CharacterExists(characterName))
	{
		const std::string error = CHARACTER_ALREADY_EXISTS;
		SendPacket(OPCODE_CREATE_CHARACTER_UNSUCCESSFUL, 1, error);
	}
	else
	{
		repository.CreateCharacter((*it)->GetAccountId(), characterName);
		SendPacket(OPCODE_CREATE_CHARACTER_SUCCESSFUL);
	}
}

void SocketManager::SendPacket(const std::string& opcode, const int argCount, ...)
{
	std::string response = std::string(CHECKSUM) + opcode;

	va_list args;
	va_start(args, argCount);

	for (auto i = 0; i < argCount; i++)
		response += (va_arg(args, std::string) + "|");
	va_end(args);

	char responseBuffer[1024];
	ZeroMemory(responseBuffer, sizeof(responseBuffer));
	strcpy_s(responseBuffer, sizeof(responseBuffer), response.c_str());
	sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
}

// PUBLIC
void SocketManager::InitializeSockets()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
        throw new std::exception(SOCKET_INIT_FAILED);
}

void SocketManager::CloseSockets()
{
    closesocket(socketS);
    WSACleanup();
}

void SocketManager::HandleTimeout()
{
    const auto it = players.begin();
    for_each(players.begin(), players.end(), [&it, this](Player& player) {
        if (GetTickCount() > player.GetLastHeartbeat() + TIMEOUT_DURATION)
        {
            std::cout << "AccountId " << player.GetAccountId() << " timed out." << "\n";
			delete(*it);
            players.erase(it);
        }
    });
}

void SocketManager::TryRecieveMessage()
{
    char buffer[1024];   
    char str[INET_ADDRSTRLEN];
    ZeroMemory(buffer, sizeof(buffer));   
    ZeroMemory(str, sizeof(str));
    if (recvfrom(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
    {
        inet_ntop(AF_INET, &(from.sin_addr), str, INET_ADDRSTRLEN);
        printf("Received message from %s:%i -  %s\n", str, from.sin_port, buffer);

        const auto checksumArrLen = 8;
        char checksumArr[checksumArrLen];
        memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));
		if (!MessagePartsEqual(checksumArr, CHECKSUM, checksumArrLen))
		{
			std::cout << "Wrong checksum. Ignoring packet.\n";
			return;
		}

        const auto opcodeArrLen = 2;
        char opcodeArr[opcodeArrLen];
        memcpy(&opcodeArr[0], &buffer[8], opcodeArrLen * sizeof(char));

        std::vector<std::string> args;
        auto bufferLength = strlen(buffer);
        if (bufferLength > 10)
        {
            std::string arg = "";
            for (unsigned int i = 10; i < bufferLength; i++)
            {
                if (buffer[i] == '|')
                {
                    args.push_back(arg);
                    arg = "";
                }
                else
                    arg += buffer[i];
            }

            std::cout << "Args:\n";
            for_each(args.begin(), args.end(), [](std::string str) { std::cout << "  " << str << "\n";  });
            std::cout << "\n";
        }

        // Login
        if (MessagePartsEqual(opcodeArr, OPCODE_CONNECT, opcodeArrLen))
        {
            const auto accountName = args[0];
            const auto password = args[1];
            const auto ipAndPort = std::string(str) + ":" + std::to_string(from.sin_port);

            Login(accountName, password, ipAndPort);
        }
        // Logout
        else if (MessagePartsEqual(opcodeArr, OPCODE_DISCONNECT, opcodeArrLen))
        {
			const auto token = args[0];

			Logout(token);
        }
        // CreateAccount
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT, opcodeArrLen))
        {
            const auto accountName = args[0];
            const auto password = args[1];

			CreateAccount(accountName, password);
        }
		// CreateCharacter
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER, opcodeArrLen))
        {
			const auto token = args[0];
			const auto characterName = args[1];

			CreateCharacter(token, characterName);
        }
    }
}
