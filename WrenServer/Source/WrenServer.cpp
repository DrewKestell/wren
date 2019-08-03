#include "stdafx.h"
#include <Constants.h>
#include <ObjectManager.h>
#include "SocketManager.h"
#include <GameTimer.h>
#include <Components/StatsComponentManager.h>
#include "Components/AIComponentManager.h"
#include "Components/PlayerComponentManager.h"

static constexpr auto CLIENT_UPDATE_FREQUENCY = 0.05f;

static ServerRepository repository{ "..\\..\\Databases\\WrenServer.db" };
static CommonRepository commonRepository{ "..\\..\\Databases\\WrenCommon.db " };

ObjectManager g_objectManager;
GameTimer m_timer;
EventHandler g_eventHandler;
StatsComponentManager g_statsComponentManager{ g_objectManager };
GameMap g_gameMap;
AIComponentManager g_aiComponentManager{ g_objectManager, g_gameMap };
PlayerComponentManager g_playerComponentManager{ g_objectManager };
SocketManager g_socketManager{ repository, commonRepository };

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

	auto updateTimer{ 0.0f };
	auto clientUpdateTimer{ 0.0f };
	m_timer.Reset();

    while (true)
    {
		while (g_socketManager.TryRecieveMessage()) {}

		// turn this off for debugging
		//socketManager.HandleTimeout();

		m_timer.Tick();
		auto deltaTime = m_timer.DeltaTime();

		updateTimer += deltaTime;
		if (updateTimer >= UPDATE_FREQUENCY)
		{
			g_objectManager.Update();
			g_aiComponentManager.Update();
			g_playerComponentManager.Update();

			PublishEvents();

			updateTimer -= UPDATE_FREQUENCY;
		}
		
		clientUpdateTimer += deltaTime;
		if (clientUpdateTimer >= CLIENT_UPDATE_FREQUENCY)
		{
			g_socketManager.UpdateClients();

			clientUpdateTimer -= CLIENT_UPDATE_FREQUENCY;
		}
    }
    
    g_socketManager.CloseSockets();    

    return 0;
}