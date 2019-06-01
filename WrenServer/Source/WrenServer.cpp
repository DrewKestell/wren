#include "stdafx.h"
#include <ObjectManager.h>
#include "SocketManager.h"
#include <GameTimer.h>

ObjectManager g_objectManager;
GameTimer m_timer;

static const auto updateInterval = 0.01666666666f;
static const auto clientUpdateInterval = 0.1f;

int main()
{
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 810, 0, 800, 800, TRUE);
    std::cout << "WrenServer initialized.\n";

    Repository repository;
    SocketManager socketManager{ repository };

	auto updateTimer{ 0.0f };
	auto clientUpdateTimer{ 0.0f };
	m_timer.Reset();

    while (true)
    {
		while (socketManager.TryRecieveMessage()) {}

		// turn this off for debugging
        //socketManager.HandleTimeout();

		m_timer.Tick();
		auto deltaTime = m_timer.DeltaTime();

		updateTimer += deltaTime;
		if (updateTimer >= updateInterval)
		{
			g_objectManager.Update(updateInterval, false);

			updateTimer -= updateInterval;
		}
		
		clientUpdateTimer += deltaTime;
		if (clientUpdateTimer >= clientUpdateInterval)
		{
			socketManager.UpdateClients();

			clientUpdateTimer -= clientUpdateInterval;
		}
    }
    
    socketManager.CloseSockets();    

    return 0;
}