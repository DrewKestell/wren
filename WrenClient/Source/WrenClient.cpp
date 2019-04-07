#include "stdafx.h"
#include "DirectXManager.h"
#include "PlayerController.h"
#include "EventHandling/Events/SystemKeyUpEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/MouseEvent.h"

static wchar_t szWindowClass[] = L"win32app";
static wchar_t szTitle[] = L"Wren Client";
static unsigned int clientWidth;
static unsigned int clientHeight;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GameTimer* timer;
DirectXManager* dxManager;
EventHandler* g_eventHandler;
SocketManager* socketManager;
ObjectManager* objectManager;
PlayerController* playerController;

unsigned int GetClientWidth() { return clientWidth; }
unsigned int GetClientHeight() { return clientHeight; }

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

		RECT rect;
		GetClientRect(hWnd, &rect);
		clientWidth = rect.right - rect.left;
		clientHeight = rect.bottom - rect.top;

        timer = new GameTimer;
		g_eventHandler = new EventHandler;
		objectManager = new ObjectManager;
		socketManager = new SocketManager;
        dxManager = new DirectXManager{ *timer, *socketManager, *objectManager };
        dxManager->Initialize(hWnd);
		playerController = new PlayerController;

        // Main game loop:
        MSG msg = { 0 };
        timer->Reset();
        while (msg.message != WM_QUIT)
        {
            // Process packets from server.
			// This method will return false when the socket buffer is empty.
			while (socketManager->TryRecieveMessage()) {}

            // Process window messages.
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            // Tick game engine
            else
            {
				playerController->Update(*timer);
                timer->Tick();
				g_eventHandler->PublishEvents();
                dxManager->DrawScene();
            }
        }

		socketManager->CloseSockets();
        return (int)msg.wParam;
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
        return -1;
    }
}

WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
	UINT scancode = (lParam & 0x00ff0000) >> 16;
	int extended = (lParam & 0x01000000) != 0;

	switch (vk)
	{
	case VK_SHIFT:
		if (MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT)
			return VK_LSHIFT;
		else
			return VK_RSHIFT;
		break;
	case VK_CONTROL:
		if (extended)
			return VK_RCONTROL;
		else
			return VK_LCONTROL;
		break;
	case VK_MENU:
		if (extended)
			return VK_RMENU;
		else
			return VK_LMENU;
		break;
	}

	throw std::exception("Something went wrong.");
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WPARAM keyCode;
    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
		break;
    case WM_LBUTTONDOWN:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::LeftMouseDownEvent, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_MBUTTONDOWN:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::MiddleMouseDownEvent, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_RBUTTONDOWN:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::RightMouseDownEvent, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_LBUTTONUP:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::LeftMouseUpEvent,(float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_MBUTTONUP:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::MiddleMouseUpEvent,(float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_RBUTTONUP:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::RightMouseUpEvent,(float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
		break;
    case WM_MOUSEMOVE:
		g_eventHandler->QueueEvent(new MouseEvent{ EventType::MouseMoveEvent,(float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
        break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_MENU:
			const auto keyCode = MapLeftRightKeys(wParam, lParam);
			g_eventHandler->QueueEvent(new SystemKeyDownEvent{ keyCode });
			break;
		}
		break;

	case WM_SYSKEYUP:
		switch (wParam)
		{
		case VK_MENU:
			const auto keyCode = MapLeftRightKeys(wParam, lParam);
			g_eventHandler->QueueEvent(new SystemKeyUpEvent{ keyCode });
			break;
		}
		break;

	case WM_KEYDOWN:		
		switch (wParam)
		{
		case VK_SHIFT:
		case VK_CONTROL:
			keyCode = MapLeftRightKeys(wParam, lParam);
			break;
		default:
			keyCode = wParam;
			break;
		}
		g_eventHandler->QueueEvent(new SystemKeyDownEvent{ keyCode });
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SHIFT:
		case VK_CONTROL:
			keyCode = MapLeftRightKeys(wParam, lParam);
			break;
		default:
			keyCode = wParam;
			break;
		}
		g_eventHandler->QueueEvent(new SystemKeyUpEvent{ keyCode });
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
        default:   // Process a normal character press.            
            auto ch = (wchar_t)wParam;
			g_eventHandler->QueueEvent(new KeyDownEvent{ ch });
            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}
