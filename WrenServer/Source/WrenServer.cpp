#include "stdafx.h"
#include <Constants.h>
#include <ObjectManager.h>
#include "SocketManager.h"
#include <GameTimer.h>

ObjectManager g_objectManager;
GameTimer m_timer;

static const auto CLIENT_UPDATE_FREQUENCY = 0.1f;

int main()
{
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 810, 0, 800, 800, TRUE);
    std::cout << "WrenServer initialized.\n\n";

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
		if (updateTimer >= UPDATE_FREQUENCY)
		{
			g_objectManager.Update();

			updateTimer -= UPDATE_FREQUENCY;
		}
		
		clientUpdateTimer += deltaTime;
		if (clientUpdateTimer >= CLIENT_UPDATE_FREQUENCY)
		{
			socketManager.UpdateClients();

			clientUpdateTimer -= CLIENT_UPDATE_FREQUENCY;
		}
    }
    
    socketManager.CloseSockets();    

    return 0;
}