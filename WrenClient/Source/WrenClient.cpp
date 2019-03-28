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
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/SystemKeyUpEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/MouseMoveEvent.h"
#include "EventHandling/Events/MouseDownEvent.h"
#include "EventHandling/Events/MouseUpEvent.h"

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Wren Client");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GameTimer* timer;
DirectXManager* dxManager;
EventHandler* eventHandler;
SocketManager* socketManager;

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
		eventHandler = new EventHandler;
        dxManager = new DirectXManager{ *timer, *socketManager, eventHandler };
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
                dxManager->DrawScene();
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

SystemKey MapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
	UINT scancode = (lParam & 0x00ff0000) >> 16;
	int extended = (lParam & 0x01000000) != 0;

	switch (vk)
	{
	case VK_SHIFT:
		if (MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT)
			return SystemKey::LeftShift;
		else
			return SystemKey::RightShift;
		break;
	case VK_CONTROL:
		if (extended)
			return SystemKey::RightControl;
		else
			return SystemKey::LeftControl;
		break;
	case VK_MENU:
		if (extended)
			return SystemKey::RightAlt;
		else
			return SystemKey::LeftAlt;
		break;
	}

	throw std::exception("Something went wrong.");
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
		break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
		eventHandler->PublishEvent(MouseDownEvent{ (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
		eventHandler->PublishEvent(MouseUpEvent{ (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_MOUSEMOVE:
		eventHandler->PublishEvent(MouseMoveEvent{ (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
        break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_MENU:
			const auto keyCode = MapLeftRightKeys(wParam, lParam);
			eventHandler->PublishEvent(SystemKeyDownEvent{ keyCode });
			break;
		}
		break;

	case WM_SYSKEYUP:
		switch (wParam)
		{
		case VK_MENU:
			const auto keyCode = MapLeftRightKeys(wParam, lParam);
			eventHandler->PublishEvent(SystemKeyUpEvent{ keyCode });
			break;
		}
		break;

	case WM_KEYDOWN:
		SystemKey keyCode;
		switch (wParam)
		{
		case VK_F1:
			keyCode = SystemKey::F1;
			break;
		case VK_F2:
			keyCode = SystemKey::F2;
			break;
		case VK_F3:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F3 });
			break;
		case VK_F4:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F4 });
			break;
		case VK_F5:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F5 });
			break;
		case VK_F6:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F6 });
			break;
		case VK_F7:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F7 });
			break;
		case VK_F8:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F8 });
			break;
		case VK_F9:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F9 });
			break;
		case VK_F10:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F10 });
			break;
		case VK_F11:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F11 });
			break;
		case VK_F12:
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::F12 });
			break;
		case 0x08: // Process a backspace.
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::Backspace });
			break;
		case 0x0A: // Process a linefeed.   
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::Linefeed });
			break;
		case 0x1B: // Process an escape. 
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::Escape });
			break;
		case 0x09: // Process a tab.          
			eventHandler->PublishEvent(SystemKeyDownEvent{ SystemKey::Tab });
			break;
		case 0x0D: // Process a carriage return.
			keyCode = SystemKey::CarriageReturn;
			break;
		case VK_SHIFT:
		case VK_CONTROL:
			keyCode = MapLeftRightKeys(wParam, lParam);
			break;
		}
		eventHandler->PublishEvent(SystemKeyDownEvent{ keyCode });
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F1:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F1 });
			break;
		case VK_F2:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F2 });
			break;
		case VK_F3:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F3 });
			break;
		case VK_F4:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F4 });
			break;
		case VK_F5:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F5 });
			break;
		case VK_F6:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F6 });
			break;
		case VK_F7:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F7 });
			break;
		case VK_F8:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F8 });
			break;
		case VK_F9:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F9 });
			break;
		case VK_F10:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F10 });
			break;
		case VK_F11:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F11 });
			break;
		case VK_F12:
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::F12 });
			break;
		case 0x08: // Process a backspace.
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::Backspace });
			break;
		case 0x0A: // Process a linefeed.   
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::Linefeed });
			break;
		case 0x1B: // Process an escape. 
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::Escape });
			break;
		case 0x09: // Process a tab.          
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::Tab });
			break;
		case 0x0D: // Process a carriage return.
			eventHandler->PublishEvent(SystemKeyUpEvent{ SystemKey::CarriageReturn });
			break;
		case VK_SHIFT:
		case VK_CONTROL:
			const auto keyCode = MapLeftRightKeys(wParam, lParam);
			eventHandler->PublishEvent(SystemKeyUpEvent{ keyCode });
			break;
		}

		break;

    case WM_CHAR:
        switch (wParam)
        {
        case 0x08: // Ignore backspace.
            break;
        case 0x0A: // Ignore linefeed.   
            break;
        case 0x1B: // Ignore escape. 
            break;
        case 0x09: // Ignore tab.          
            break;
        case 0x0D: // Ignore carriage return.
            break;
        default: // Process a normal character press.            
            auto ch = (wchar_t)wParam;
			eventHandler->PublishEvent(KeyDownEvent{ ch });
            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    socketManager->CloseSockets();

    return 0;
}
