
#include "stdafx.h"
#include <GameTimer.h>
#include <Components/StatsComponentManager.h>
#include "Components/AIComponentManager.h"
#include "Components/PlayerComponentManager.h"
#include "Components/SkillComponentManager.h"
#include "Components/InventoryComponentManager.h"

void PublishEvents(EventHandler& eventHandler)
{
	std::queue<std::unique_ptr<const Event>>& eventQueue = eventHandler.GetEventQueue();
	std::list<Observer*>& observers = eventHandler.GetObservers();
	while (!eventQueue.empty())
	{
		auto event = std::move(eventQueue.front());
		eventQueue.pop();

		for (auto it = observers.begin(); it != observers.end(); it++)
		{
			if ((*it)->HandleEvent(event.get()))
				break;
		}
	}
}

int main()
{
	static EventHandler eventHandler;
	static ObjectManager objectManager;
	static GameMap gameMap;
	static ServerComponentOrchestrator componentOrchestrator;
	static ServerRepository serverRepository{ "..\\..\\Databases\\WrenServer.db" };
	static CommonRepository commonRepository{ "..\\..\\Databases\\WrenCommon.db " };
	static ServerSocketManager socketManager{ eventHandler, gameMap, objectManager, componentOrchestrator, serverRepository, commonRepository };
	static AIComponentManager aiComponentManager{ eventHandler, objectManager, gameMap, componentOrchestrator, socketManager };
	static PlayerComponentManager playerComponentManager{ eventHandler, objectManager, gameMap, componentOrchestrator, socketManager };
	static SkillComponentManager skillComponentManager{ eventHandler, objectManager, componentOrchestrator, socketManager };
	static StatsComponentManager statsComponentManager{ eventHandler, objectManager };
	static InventoryComponentManager inventoryComponentManager{ eventHandler, objectManager };
	componentOrchestrator.InitializeComponentManagers(&aiComponentManager, &playerComponentManager, &skillComponentManager, &statsComponentManager, &inventoryComponentManager);
	socketManager.Initialize();

    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 810, 0, 800, 800, TRUE);
    std::cout << "WrenServer initialized.\n\n";

	GameTimer timer;
	timer.Reset();
	auto updateTimer = 0.0f;
	
    while (true)
    {
		timer.Tick();

		socketManager.ProcessPackets();

		// turn this off for debugging
		//socketManager.HandleTimeout();

		const auto deltaTime = timer.DeltaTime();

		updateTimer += deltaTime;
		if (updateTimer >= UPDATE_FREQUENCY)
		{
			aiComponentManager.Update();
			playerComponentManager.Update();
			objectManager.Update();
			
			PublishEvents(eventHandler);

			socketManager.UpdateClients();

			updateTimer -= UPDATE_FREQUENCY;
		}
    }
    
    socketManager.CloseSockets();    

    return 0;
}