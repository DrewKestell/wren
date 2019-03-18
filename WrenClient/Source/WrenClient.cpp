#include "SocketManager.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <exception>
#include <tuple>
#include "DirectXManager.h"
#include "GameTimer.h"
#include "LoginState.h"

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Wren Client");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GameTimer* timer;
DirectXManager* dxManager;
SocketManager* socketManager;

int mouseX = 0;
int mouseY = 0;

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    try
    {
        // Allocate Console
#ifdef _DEBUG
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        HWND consoleWindow = GetConsoleWindow();
        MoveWindow(consoleWindow, 0, 600, 800, 400, TRUE);
        std::cout << "WrenClient initialized.\n";
#endif

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
            0,
            0,
            800,
            600,
            NULL,
            NULL,
            hInstance,
            NULL
        );
        if (!hWnd)
            throw std::exception("CreateWindow failed.");

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        timer = new GameTimer;
        socketManager = new SocketManager;
        dxManager = new DirectXManager{ *timer, *socketManager };
        dxManager->Initialize(hWnd);

        // Main game loop:
        MSG msg = { 0 };

        timer->Reset();

        while (msg.message != WM_QUIT)
        {
            // If there are socket messages then process them.
            bool morePackets = true;
            while (morePackets)
            {
                const auto result = socketManager->TryRecieveMessage();
                const auto messageType = std::get<0>(result);
                if (messageType == "SOCKET_BUFFER_EMPTY")
                    morePackets = false;
                else
                    dxManager->HandleMessage(result);
            }
            // If there are Window messages then process them.
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            // Otherwise, do animation/game stuff.
            else
            {
                timer->Tick();
                dxManager->DrawScene(mouseX, mouseY);
            }
        }
        return (int)msg.wParam;
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
        return -1;
    }
}

void OnMouseMove(WPARAM wParam, FLOAT x, FLOAT y)
{
    mouseX = x;
    mouseY = y;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        dxManager->MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        dxManager->MouseUp();
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_CHAR:
        switch (wParam)
        {
        case 0x08: // Process a backspace.
            dxManager->OnBackspace();
            break;

        case 0x0A: // Process a linefeed.           
            break;

        case 0x1B: // Process an escape.            
            break;

        case 0x09: // Process a tab.          
            dxManager->OnTab();
            break;

        case 0x0D: // Process a carriage return.             
            break;

        default: // Process a normal character press.            
            auto ch = (TCHAR)wParam;
            dxManager->OnKeyPress(ch);
            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    socketManager->CloseSockets();

    return 0;
}
