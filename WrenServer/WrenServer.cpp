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
    DWORD LastHeartbeat;
};

constexpr char CHECKSUM[8] = { '6', '5', '8', '3', '6', '2', '1', '6' };

constexpr char OPCODE_CONNECT[2] =                     { '0', '0' };
constexpr char OPCODE_DISCONNECT[2] =                  { '0', '1' };
constexpr char OPCODE_LOGIN_SUCCESSFUL[2] =            { '0', '2' };
constexpr char OPCODE_LOGIN_UNSUCCESSFUL[2] =          { '0', '3' };
constexpr char OPCODE_CREATE_ACCOUNT[2] =              { '0', '4' };
constexpr char OPCODE_CREATE_ACCOUNT_SUCCESSFUL[2] =   { '0', '5' };
constexpr char OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL[2] = { '0', '6' };

constexpr auto TIMEOUT_DURATION = 30000; // 30000ms == 30s

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

bool MessagePartsEqual(const char * first, const char * second, int length)
{
    for (auto i = 0; i < length; i++)
    {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

void HandleTimeout()
{
    std::vector<Player>::const_iterator it = PLAYERS.begin();
    std::for_each(PLAYERS.begin(), PLAYERS.end(), [&it](Player player) {
        if (GetTickCount() > player.LastHeartbeat + TIMEOUT_DURATION)
        {
            std::cout << player.Name << " timed out." << "\n";
            PLAYERS.erase(it);
        }
    });
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

            const auto checksumArrLen = 8;
            char checksumArr[checksumArrLen];
            memcpy(&checksumArr[0], &buffer[0], checksumArrLen * sizeof(char));
            if (!MessagePartsEqual(checksumArr, CHECKSUM, checksumArrLen))
                continue;

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
                std::for_each(args.begin(), args.end(), [](std::string str) { std::cout << "  " << str << "\n";  });
                std::cout << "\n";
            }

            // connect
            if (MessagePartsEqual(opcodeArr, OPCODE_CONNECT, opcodeArrLen))
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
                        player.LastHeartbeat = GetTickCount();
                        PLAYERS.push_back(player);

                        strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::string(OPCODE_LOGIN_SUCCESSFUL)).c_str());
                        sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                        std::cout << player.Name << " connected to the server.\n";
                    }
                        
                }
                // account doesn't exist
                else
                    error = "Incorrect Username.";

                if (error != "")
                {
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::string(OPCODE_LOGIN_UNSUCCESSFUL) + error).c_str());
                    sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                }
            }
            // disconnect
            else if (MessagePartsEqual(opcodeArr, OPCODE_DISCONNECT, opcodeArrLen))
            {
                auto playerName = args[0];

                auto it = std::find_if(PLAYERS.begin(), PLAYERS.end(), [&playerName](const Player& player) { return player.Name == playerName; });
                if (it != PLAYERS.end())
                    PLAYERS.erase(it);
            }
            else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT, opcodeArrLen))
            {
                auto accountName = args[0];
                auto password = args[1];

                auto it = std::find_if(ACCOUNTS.begin(), ACCOUNTS.end(), [&accountName](const Account& account) { return account.AccountName == accountName; });
                std::string error = "Account already exists.";
                // account already exists
                if (it != ACCOUNTS.end())
                {
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
                        break;
                    }

                    // save the new account in the db
                    strcpy_s(responseBuffer, (std::string(CHECKSUM) + std::string(OPCODE_CREATE_ACCOUNT_SUCCESSFUL)).c_str());
                    sendto(socketS, responseBuffer, sizeof(responseBuffer), 0, (sockaddr*)&from, fromlen);
                }
            }
        }
        HandleTimeout();
        Sleep(500);
    }
    
    closesocket(socketS);
    WSACleanup();

    return 0;
}