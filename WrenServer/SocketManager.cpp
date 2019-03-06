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

constexpr auto SOCKET_INIT_FAILED = "Failed to initialize sockets.";

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
bool SocketManager::MessagePartsEqual(const char* first, const char* second, int length)
{
    for (auto i = 0; i < length; i++)
    {
        if (first[i] != second[i])
            return false;
    }
    return true;
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
    std::vector<Player>::const_iterator it = players.begin();
    for_each(players.begin(), players.end(), [&it, this](Player& player) {
        if (GetTickCount() > player.GetLastHeartbeat() + TIMEOUT_DURATION)
        {
            std::cout << player.GetName() << " timed out." << "\n";
            players.erase(it);
        }
    });
}

void SocketManager::Login(
    const std::string& accountName,
    const std::string& password,
    const std::string characterName,
    const std::string ipAndPort,
    const char* inBuffer,
    char* outBuffer)
{
    std::string error;
    auto account = repository.GetAccount(accountName);
    if (account)
    {
        auto passwordArr = password.c_str();
        if (crypto_pwhash_str_verify(account->GetPassword().c_str(), passwordArr, strlen(passwordArr)) != 0)
            error = "Incorrect Password.";
        else
        {
            Player player
            {
                characterName,
                ipAndPort,
                GetTickCount()
            };
            players.push_back(player);

            auto responseMessage = (std::string(CHECKSUM) + std::string(OPCODE_LOGIN_SUCCESSFUL)).c_str();
            strcpy_s(outBuffer, sizeof(outBuffer), responseMessage);
            sendto(socketS, outBuffer, sizeof(outBuffer), 0, (sockaddr*)&from, fromlen);
            std::cout << player.GetName() << " connected to the server.\n";
        }

    }
    // account doesn't exist
    else
        error = "Incorrect Username.";

    if (error != "")
    {
        auto responseMessage = (std::string(CHECKSUM) + std::string(OPCODE_LOGIN_SUCCESSFUL)).c_str();
        strcpy_s(outBuffer, sizeof(outBuffer), responseMessage);
        sendto(socketS, outBuffer, sizeof(outBuffer), 0, (sockaddr*)&from, fromlen);
    }
}

void SocketManager::TryRecieveMessage()
{
    char buffer[1024];
    char responseBuffer[1024];
    char str[INET_ADDRSTRLEN];
    ZeroMemory(buffer, sizeof(buffer));
    ZeroMemory(responseBuffer, sizeof(responseBuffer));
    ZeroMemory(str, sizeof(str));
    if (recvfrom(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
    {
        inet_ntop(AF_INET, &(from.sin_addr), str, INET_ADDRSTRLEN);
        printf("Received message from %s:%i -  %s\n", str, from.sin_port, buffer);

        const auto checksumArrLen = 8;
        char checksumArr[checksumArrLen];
        memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));
        if (!MessagePartsEqual(checksumArr, CHECKSUM, checksumArrLen))
            return;

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

        // connect
        if (MessagePartsEqual(opcodeArr, OPCODE_CONNECT, opcodeArrLen))
        {
            const auto accountName = args[0];
            const auto password = args[1];
            const auto characterName = args[2];
            const auto ipAndPort = std::string(str) + ":" + std::to_string(from.sin_port);

            Login(accountName, password, characterName, ipAndPort, buffer, responseBuffer);
        }
        // disconnect
        else if (MessagePartsEqual(opcodeArr, OPCODE_DISCONNECT, opcodeArrLen))
        {
            auto playerName = args[0];

            auto it = find_if(players.begin(), players.end(), [&playerName](Player& player) { return player.GetName() == playerName; });
            if (it != players.end())
                players.erase(it);
        }
        // create account
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT, opcodeArrLen))
        {
            auto accountName = args[0];
            auto password = args[1];

            // account already exists
            if (repository.AccountExists(accountName))
            {
                std::string error = "Account already exists.";
                strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::string(OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL) + error).c_str());
                sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
            }
            // create the account
            else
            {
                char hashedPassword[crypto_pwhash_STRBYTES];
                auto passwordArr = password.c_str();
                if (crypto_pwhash_str(
                    hashedPassword,
                    passwordArr,
                    strlen(passwordArr),
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
                {
                    std::cout << "Ran out of memory while hashing password.";
                    return;
                }

                repository.CreateAccount(accountName, hashedPassword);

                std::string response = "Account successfully created.";
                strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::string(OPCODE_CREATE_ACCOUNT_SUCCESSFUL) + response).c_str());
                sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
            }
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER, opcodeArrLen))
        {

        }
    }
}

