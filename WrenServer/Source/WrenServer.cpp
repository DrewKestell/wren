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
	m_timer.Reset();

    while (true)
    {
		/*while (socketManager.TryRecieveMessage()) {}
        socketManager.HandleTimeout();*/

		m_timer.Tick();

		updateTimer += m_timer.DeltaTime();
		if (updateTimer >= 0.001666666666f)
		{
			if (updateTimer >= 0.002)
				std::cout << "wtf\n";
			//std::cout << "Server deltaTime: " << updateTimer << std::endl;
			//g_objectManager.Update(updateTimer);

			updateTimer = 0.0f;
		}
    }
    
    socketManager.CloseSockets();    

    return 0;
}