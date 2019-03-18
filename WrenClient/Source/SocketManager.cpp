#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include "SocketManager.h"

constexpr auto SOCKET_INIT_FAILED = "Failed to initialize sockets.";
constexpr auto CLIENT_PORT_NUMBER = 27015;
constexpr auto SERVER_PORT_NUMBER = 27016;

// CONSTRUCTOR
SocketManager::SocketManager()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
        throw new std::exception(SOCKET_INIT_FAILED);

    local.sin_family = AF_INET;
    local.sin_port = htons(CLIENT_PORT_NUMBER);
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
}

void SocketManager::CloseSockets()
{
    closesocket(socketC);
    WSACleanup();
}

std::tuple<std::string, std::string, std::vector<std::string>*> SocketManager::TryRecieveMessage()
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
            return std::make_tuple("WRONG_CHECKSUM", "", nullptr);
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

        // CreateAccount Unsuccessful
        if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Failed to create account. Reason: " + error + "\n";
            return std::make_tuple("CREATE_ACCOUNT_FAILED", error, nullptr);
        }
        // CreateAccount Successful
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_ACCOUNT_SUCCESSFUL, opcodeArrLen))
        {
            std::cout << "Successfully created account.\n";
            return std::make_tuple("CREATE_ACCOUNT_SUCCESS", "", nullptr);
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_SUCCESSFUL, opcodeArrLen))
        {
            const auto token = args[0];
            const auto characterString = args[1];
            std::vector<std::string>* characterList = new std::vector<std::string>;
            std::string arg = "";
            for (auto i = 0; i < characterString.length(); i++)
            {
                if (characterString[i] == ';')
                {
                    characterList->push_back(arg);
                    arg = "";
                }
                else
                    arg += characterString[i];
            }

            std::cout << "Login successful. Token received: " + token + "\n";
            return std::make_tuple("LOGIN_SUCCESS", token, characterList);
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_LOGIN_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Login failed. Reason: " + error + "\n";
            return std::make_tuple("LOGIN_FAILED", error, nullptr);
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_SUCCESSFUL, opcodeArrLen))
        {
            std::cout << "Character creation succesful.";
            return std::make_tuple("CREATE_CHARACTER_SUCCESS", "", nullptr);
        }
        else if (MessagePartsEqual(opcodeArr, OPCODE_CREATE_CHARACTER_UNSUCCESSFUL, opcodeArrLen))
        {
            const auto error = args[0];
            std::cout << "Character creation failed. Reason: " + error + "\n";
            return std::make_tuple("CREATE_CHARACTER_FAILED", error, nullptr);
        }
        else
            return std::make_tuple("NOT_IMPLEMENTED", "", nullptr);
    }
    return std::make_tuple("SOCKET_BUFFER_EMPTY", "", nullptr);
}