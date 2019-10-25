#include "stdafx.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/MouseEvent.h"
#include "Game.h"

static wchar_t szWindowClass[] = L"win32app";
static wchar_t szTitle[] = L"Wren Client";
static EventHandler eventHandler;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!XMVerifyCPUSupport())
		return 1;

	const HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	static ObjectManager objectManager;
	static RenderComponentManager renderComponentManager{ eventHandler, objectManager };
	static StatsComponentManager statsComponentManager{ eventHandler, objectManager };
	static InventoryComponentManager inventoryComponentManager{ eventHandler, objectManager };
	static ClientRepository clientRepository{ "..\\..\\Databases\\WrenClient.db" };
	static CommonRepository commonRepository{ "..\\..\\Databases\\WrenCommon.db" };
	static ClientSocketManager socketManager{ eventHandler };
	static auto game = std::make_unique<Game>(eventHandler, objectManager, renderComponentManager, statsComponentManager, inventoryComponentManager, clientRepository, commonRepository, socketManager);
	socketManager.SetGamePointer(game.get()); // this is a hack to get ping/pong working. refactor.

	// Register class
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	if (!RegisterClassEx(&wcex))
		return 1;

	// Create window
	RECT rc = { 0, 0, long{ static_cast<int>(CLIENT_WIDTH) }, long{ static_cast<int>(CLIENT_HEIGHT) } };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"Direct3D_Win32_Game1WindowClass", L"Direct3D Win32 Game1", WS_POPUP,
	// to default to fullscreen.
	HWND hWnd = CreateWindowExW(
		0,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	if (!hWnd)
		return 1;

	// TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.
	ShowWindow(hWnd, nCmdShow);

	// Wrap the WindowPtr in our Game
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(game.get()));

	GetClientRect(hWnd, &rc);
	game->Initialize(hWnd, rc.right - rc.left, rc.bottom - rc.top);

	// Main message loop
	MSG msg{ nullptr };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			game->Tick();
		}
	}

	game.reset();

	CoUninitialize();

	return (int)msg.wParam;
}

WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
	const UINT scancode = (lParam & 0x00ff0000) >> 16;
	const int extended = (lParam & 0x01000000) != 0;

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
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;

	auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	WPARAM keyCode{ 0 };
	std::unique_ptr<Event> e;
	switch (message)
	{
	case WM_PAINT:
		if (s_in_sizemove && game)
		{
			game->Tick();
		}
		else
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_MOVE:
		if (game)
		{
			game->OnWindowMoved();
		}
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && game)
					game->OnSuspending();
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && game)
				game->OnResuming();
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && game)
		{
			game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		else if (s_in_sizemove && game)
		{
			// figure out what's going on with the weird resizing behavior while dragging...
			game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (game)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

	case WM_GETMINMAXINFO:
	{
		auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;

	case WM_ACTIVATEAPP:
		if (game)
		{
			if (wParam)
			{
				game->OnActivated();
			}
			else
			{
				game->OnDeactivated();
			}
		}
		break;

	case WM_POWERBROADCAST:
		switch (wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend && game)
				game->OnSuspending();
			s_in_suspend = true;
			return TRUE;

		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend && game)
					game->OnResuming();
				s_in_suspend = false;
			}
			return TRUE;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		e = std::make_unique<MouseEvent>(EventType::LeftMouseDown, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_MBUTTONDOWN:
		e = std::make_unique<MouseEvent>(EventType::MiddleMouseDown, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_RBUTTONDOWN:
		e = std::make_unique<MouseEvent>(EventType::RightMouseDown, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_LBUTTONUP:
		e = std::make_unique<MouseEvent>(EventType::LeftMouseUp, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_MBUTTONUP:
		e = std::make_unique<MouseEvent>(EventType::MiddleMouseUp, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_RBUTTONUP:
		e = std::make_unique<MouseEvent>(EventType::RightMouseUp, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);
		break;
	case WM_MOUSEMOVE:
		e = std::make_unique<MouseEvent>(EventType::MouseMove, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		eventHandler.QueueEvent(e);

		if (s_in_sizemove)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// Implements the classic ALT+ENTER fullscreen toggle
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

				ShowWindow(hWnd, SW_SHOWNORMAL);

				SetWindowPos(hWnd, nullptr, 0, 0, static_cast<int>(CLIENT_WIDTH), static_cast<int>(CLIENT_HEIGHT), SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}
		else
		{
			switch (wParam)
			{
			case VK_MENU:
				const auto key = MapLeftRightKeys(wParam, lParam);
				e = std::make_unique<SystemKeyDownEvent>(key);
				eventHandler.QueueEvent(e);
				break;
			}
		}

		break;

	case WM_SYSKEYUP:
		switch (wParam)
		{
		case VK_MENU:
			const auto key = MapLeftRightKeys(wParam, lParam);
			e = std::make_unique<SystemKeyUpEvent>(key);
			eventHandler.QueueEvent(e);
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
		e = std::make_unique<SystemKeyDownEvent>(keyCode);
		eventHandler.QueueEvent(e);
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
		e = std::make_unique<SystemKeyUpEvent>(keyCode);
		eventHandler.QueueEvent(e);
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
			auto ch = static_cast<wchar_t>(wParam);
			e = std::make_unique<KeyDownEvent>(ch);
			eventHandler.QueueEvent(e);
			break;
		}

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		// Primarily useful for avoiding the beep on ALT + ENTER.
		return MAKELRESULT(0, MNC_CLOSE);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
