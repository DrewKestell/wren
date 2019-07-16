#include "stdafx.h"
#include <Constants.h>
#include <ObjectManager.h>
#include "SocketManager.h"
#include <GameTimer.h>
#include <Components/StatsComponentManager.h>
#include "Components/AIComponentManager.h"

static const auto CLIENT_UPDATE_FREQUENCY = 0.05f;

ObjectManager g_objectManager;
GameTimer m_timer;
EventHandler g_eventHandler;
StatsComponentManager g_statsComponentManager{ g_objectManager };
AIComponentManager g_aiComponentManager{ g_objectManager };

void PublishEvents()
{
	auto eventQueue = g_eventHandler.eventQueue;
	while (!eventQueue->empty())
	{
		const auto event = eventQueue->front();
		eventQueue->pop();

		for (auto it = g_eventHandler.observers->begin(); it != g_eventHandler.observers->end(); it++)
		{
			if ((*it)->HandleEvent(event))
				break;
		}

	}
}

int main()
{
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 810, 0, 800, 800, TRUE);
    std::cout << "WrenServer initialized.\n\n";

	ServerRepository repository{ "WrenServer.db" };
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
			g_aiComponentManager.Update();

			PublishEvents();

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