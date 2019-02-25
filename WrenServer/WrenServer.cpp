#include <winsock2.h>
#include <Ws2tcpip.h>
#include "windows.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

class Account
{
public:
    std::string AccountName;
    std::string Password;
};

class Player
{
public:
    std::string Name;
    std::string IPAndPort;
};

constexpr auto CHECKSUM = "65836216";

constexpr auto OPCODE_CONNECT = 0;
constexpr auto OPCODE_DISCONNECT = 1;
constexpr auto OPCODE_LOGIN_SUCCESSFUL = 2;
constexpr auto OPCODE_LOGIN_UNSUCCESSFUL = 3;

std::vector<Account> ACCOUNTS;
std::vector<Player> PLAYERS;

bool InitializeSockets()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
}

void InitializeAccounts()
{
    Account bloog;
    bloog.AccountName = "Bloog";
    bloog.Password = "password";
    ACCOUNTS.push_back(bloog);
}

int main()
{
    InitializeAccounts();
    InitializeSockets();

    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(27015);
    local.sin_addr.s_addr = INADDR_ANY;

    sockaddr_in from;
    int fromlen = sizeof(from);

    SOCKET socketS;
    socketS = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketS, (sockaddr*)&local, sizeof(local));

    DWORD nonBlocking = 1;
    ioctlsocket(socketS, FIONBIO, &nonBlocking);

    while (true)
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
            auto checksum = std::stoi(checksumArr, nullptr);
            std::cout << "Checksum: " << checksum;
            std::cout << "\n";

            const auto opcodeArrLen = 2;
            char opcodeArr[opcodeArrLen];
            memcpy(&opcodeArr[0], &buffer[8], opcodeArrLen * sizeof(char));
            auto opcode = std::stoi(opcodeArr, nullptr);
            std::cout << "Opcode: " << opcode;
            std::cout << "\n";

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
                std::for_each(args.begin(), args.end(), [](std::string str) { std::cout << "  " << str << "\n";  });
                std::cout << "\n";
            }

            // connect
            if (opcode == OPCODE_CONNECT)
            {
                auto accountName = args[0];
                auto password = args[1];

                auto it = std::find_if(ACCOUNTS.begin(), ACCOUNTS.end(), [&accountName](const Account& account) { return account.AccountName == accountName; });
                std::string error;
                // found the account
                if (it != ACCOUNTS.end())
                {
                    auto account = *it;

                    // password matches
                    if (password == account.Password)
                    {
                        Player player;
                        player.Name = args[2];
                        player.IPAndPort = std::string(str) + ":" + std::to_string(from.sin_port);
                        PLAYERS.push_back(player);

                        strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::to_string(OPCODE_LOGIN_SUCCESSFUL)).c_str());
                        sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                    }
                    // incorrect password
                    else
                        error = "Incorrect Password.";
                }
                // account doesn't exist
                else
                    error = "Incorrect Username.";

                if (error != "")
                {
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::to_string(OPCODE_LOGIN_UNSUCCESSFUL) + error).c_str());
                    sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                }
            }
            // disconnect
            else if (opcode == OPCODE_DISCONNECT)
            {
                auto playerName = args[0];

                auto it = std::find_if(PLAYERS.begin(), PLAYERS.end(), [&playerName](const Player& player) { return player.Name == playerName; });
                if (it != PLAYERS.end())
                    PLAYERS.erase(it);
            }
        }
        Sleep(500);
    }
    
    closesocket(socketS);
    WSACleanup();

    return 0;
}