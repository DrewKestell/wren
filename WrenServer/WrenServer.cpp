#include <winsock2.h>
#include <Ws2tcpip.h>
#include <sodium.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "windows.h"

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
constexpr auto OPCODE_CREATE_ACCOUNT = 4;
constexpr auto OPCODE_CREATE_ACCOUNT_SUCCESSFUL = 5;
constexpr auto OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL = 6;

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
    bloog.Password = "$argon2id$v=19$m=65536,t=2,p=1$gonM1QLOO91yL152cRJwPA$M4f57Y/D16IQGUxFOfWYVhDm4ZFFnO1bYXTrJ9GwoSo";
    ACCOUNTS.push_back(bloog);
}

std::string PadOpcode(int opcode)
{
    if (opcode < 10)
        return "0" + std::to_string(opcode);
    return std::to_string(opcode);
}

int main()
{
    InitializeAccounts();
    InitializeSockets();
    sodium_init();

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

            // this isn't null terminated or something...
            const auto checksumArrLen = 8;
            char checksumArr[checksumArrLen];
            memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));

            auto foo = std::string(checksumArr);
            auto bar = std::string(CHECKSUM);
            if (foo != bar)
                continue;

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

                    auto passwordArr = password.c_str();
                    if (crypto_pwhash_str_verify(account.Password.c_str(), passwordArr, strlen(passwordArr)) != 0)
                        error = "Incorrect Password.";
                    else
                    {
                        Player player;
                        player.Name = args[2];
                        player.IPAndPort = std::string(str) + ":" + std::to_string(from.sin_port);
                        PLAYERS.push_back(player);

                        strcpy_s(responseBuffer, (std::string(CHECKSUM) + PadOpcode(OPCODE_LOGIN_SUCCESSFUL)).c_str());
                        sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                    }
                        
                }
                // account doesn't exist
                else
                    error = "Incorrect Username.";

                if (error != "")
                {
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + PadOpcode(OPCODE_LOGIN_UNSUCCESSFUL) + error).c_str());
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
            else if (opcode == OPCODE_CREATE_ACCOUNT)
            {
                auto accountName = args[0];
                auto password = args[1];

                auto it = std::find_if(ACCOUNTS.begin(), ACCOUNTS.end(), [&accountName](const Account& account) { return account.AccountName == accountName; });
                std::string error = "Account already exists.";
                // account already exists
                if (it != ACCOUNTS.end())
                {
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + PadOpcode(OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL) + error).c_str());
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
                        /* out of memory */
                    }
                    // save the new account in the db
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + PadOpcode(OPCODE_CREATE_ACCOUNT_SUCCESSFUL)).c_str());
                    sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                }
            }
        }
        Sleep(500);
    }
    
    closesocket(socketS);
    WSACleanup();

    return 0;
}