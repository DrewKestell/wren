#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>

constexpr auto CHECKSUM = "65836216";

constexpr auto OPCODE_LOGIN_SUCCESSFUL = 0;
constexpr auto OPCODE_LOGIN_UNSUCCESSFUL = 1;

void InitWinsock()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int main()
{
    InitWinsock();

    SOCKET socketC;
    struct sockaddr_in serverInfo;
    int len = sizeof(serverInfo);
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(27015);
    inet_pton(AF_INET, "127.0.0.1", &serverInfo.sin_addr);

    socketC = socket(AF_INET, SOCK_DGRAM, 0);

    DWORD nonBlocking = 1;
    ioctlsocket(socketC, FIONBIO, &nonBlocking);
    while (true)
    {
        if (GetKeyState('X') & 0x8000)
            break;

        std::string packet = CHECKSUM;
        if (GetKeyState('Q') & 0x8000)
            packet += "04Bloog|password|";
        else if (GetKeyState('W') & 0x8000)
            packet += "07Bloogson|";
        else if (GetKeyState('E') & 0x8000)
            packet += "00Bloog|password|Bloog|";
        else if (GetKeyState('R') & 0x8000)
            packet += "01";

        char buffer[1024];
        ZeroMemory(buffer, sizeof(buffer));
        char packetBuffer[1024];
        ZeroMemory(packetBuffer, sizeof(packetBuffer));
        auto packetArr = packet.c_str();
        memcpy(&packetBuffer[0], &packetArr[0], strlen(packetArr) * sizeof(char));
        sendto(socketC, packetBuffer, sizeof(packetBuffer), 0, (sockaddr*)&serverInfo, len);

        while (recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &len) != SOCKET_ERROR)
        {
            printf("Receive response from server: %s\n", buffer);
            ZeroMemory(buffer, sizeof(buffer));
        }
    }
    closesocket(socketC);
    WSACleanup();

    return 0;
}