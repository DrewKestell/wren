#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string>
#include <exception>
#include "DirectXManager.h"
#include "GameTimer.h"
#include "LoginState.h"

constexpr auto CHECKSUM = "65836216";
constexpr char OPCODE_CONNECT[2] = { '0', '0' };

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Wren Client");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GameTimer timer;

int mouseX = 0;
int mouseY = 0;

bool accountNameInputActive = false;
bool passwordInputActive = false;
bool loginButtonPressed = false;

int accountNameInputIndex = 0;
char accountNameInputValue[32];
int passwordInputIndex = 0;
char passwordInputValue[32];

SOCKET socketC;
struct sockaddr_in serverInfo;
int len;

LoginState loginState = LoginScreen;

void InitWinsock()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        throw std::exception("RegisterClassEx failed.");

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (!hWnd)
        throw std::exception("CreateWindow failed.");

    ZeroMemory(accountNameInputValue, sizeof(accountNameInputValue));
    ZeroMemory(passwordInputValue, sizeof(passwordInputValue));

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    GameTimer timer;
    DirectXManager dxManager{ timer };
    dxManager.Initialize(hWnd);

    // initialize socket connection
    InitWinsock();
    len = sizeof(serverInfo);
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(27015);
    inet_pton(AF_INET, "127.0.0.1", &serverInfo.sin_addr);
    socketC = socket(AF_INET, SOCK_DGRAM, 0);
    DWORD nonBlocking = 1;
    ioctlsocket(socketC, FIONBIO, &nonBlocking);

    // Main game loop:
    MSG msg = { 0 };

    timer.Reset();

    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.
        else
        {
            timer.Tick();
            dxManager.DrawScene(mouseX, mouseY, accountNameInputActive, passwordInputActive, loginButtonPressed, accountNameInputValue, passwordInputValue, loginState);
        }
    }
    return (int)msg.wParam;
}

bool IsWithinRect(int mouseX, int mouseY, int left, int top, int right, int bottom)
{
    return mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom;
}

void OnMouseDown(WPARAM wParam, int x, int y)
{
    if (accountNameInputActive)
        accountNameInputActive = false;
    if (passwordInputActive)
        passwordInputActive = false;

    if (IsWithinRect(x, y, 140, 16, 400, 40))
        accountNameInputActive = true;
    else if (IsWithinRect(x, y, 140, 47, 400, 71))
        passwordInputActive = true;
    else if (IsWithinRect(x, y, 140, 90, 220, 114))
    {
        loginButtonPressed = true;
        std::string packet = CHECKSUM;
        packet += "00" + std::string(accountNameInputValue) + "|" + std::string(passwordInputValue) + "|";;

        char packetBuffer[1024];
        ZeroMemory(packetBuffer, sizeof(packetBuffer));
        auto packetArr = packet.c_str();
        memcpy(&packetBuffer[0], &packetArr[0], strlen(packetArr) * sizeof(char));
        auto foo = sendto(socketC, packetBuffer, sizeof(packetBuffer), 0, (sockaddr*)&serverInfo, len);
    }
}

void OnMouseUp(WPARAM wParam, int x, int y)
{
    if (loginButtonPressed)
        loginButtonPressed = false;
}

void OnMouseMove(WPARAM wParam, int x, int y)
{
    mouseX = x;
    mouseY = y;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_CHAR:
        switch (wParam)
        {
        case 0x08:
            if (accountNameInputActive)
            {
                if (accountNameInputIndex == 0)
                    break;

                accountNameInputValue[accountNameInputIndex - 1] = 0;
                accountNameInputIndex--;
            }
            else if (passwordInputActive)
            {
                if (passwordInputIndex == 0)
                    break;

                passwordInputValue[passwordInputIndex - 1] = 0;
                passwordInputIndex--;
            }

            break;

        case 0x0A:
            // Process a linefeed. 
            break;

        case 0x1B:
            // Process an escape. 
            break;

        case 0x09:
            if (!accountNameInputActive && !passwordInputActive)
                accountNameInputActive = true;
            else if (accountNameInputActive)
            {
                accountNameInputActive = false;
                passwordInputActive = true;
            }
            else
            {
                accountNameInputActive = true;
                passwordInputActive = false;
            }
            break;

        case 0x0D:
            // Process a carriage return. 
            break;

        default:
            auto ch = (TCHAR)wParam;

            if (accountNameInputActive)
            {
                if (accountNameInputIndex > 32)
                    break;

                accountNameInputValue[accountNameInputIndex] = ch;
                accountNameInputIndex++;
            }
            else if (passwordInputActive)
            {
                if (passwordInputIndex > 32)
                    break;

                passwordInputValue[passwordInputIndex] = ch;
                passwordInputIndex++;
            }

            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    closesocket(socketC);
    WSACleanup();

    return 0;
}
