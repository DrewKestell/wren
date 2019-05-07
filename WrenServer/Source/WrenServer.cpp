#include "stdafx.h"
#include <ObjectManager.h>
#include "SocketManager.h"
#include <GameTimer.h>

ObjectManager g_objectManager;
GameTimer m_timer;

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
        socketManager.HandleTimeout();

		m_timer.Tick();
		auto deltaTime = m_timer.DeltaTime();

		updateTimer += deltaTime;
		if (updateTimer >= 0.01666666666f)
		{
			g_objectManager.Update(0.01666666666f);

			updateTimer -= 0.01666666666f;
		}
		
		clientUpdateTimer += deltaTime;
		if (clientUpdateTimer >= 0.1f)
		{
			socketManager.UpdateClients();

			clientUpdateTimer -= 0.1f;
		}
    }
    
    socketManager.CloseSockets();    

    return 0;
}