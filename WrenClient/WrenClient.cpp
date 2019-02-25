#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

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
    while (true)
    {
        char buffer[1024];
        ZeroMemory(buffer, sizeof(buffer));
        printf("Please input your message: ");
        scanf_s("%s", buffer, _countof(buffer));
        if (strcmp(buffer, "exit") == 0)
            break;
        if (sendto(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, len) != SOCKET_ERROR)
        {
            if (recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &len) != SOCKET_ERROR)
            {
                printf("Receive response from server: %s\n", buffer);
            }
        }
    }
    closesocket(socketC);
    WSACleanup();

    return 0;
}