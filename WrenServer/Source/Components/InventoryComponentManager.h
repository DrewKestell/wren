#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "InventoryComponent.h"
#include "ObjectManager.h"
#include "../ServerSocketManager.h"

constexpr unsigned int MAX_INVENTORYCOMPONENTS_SIZE = 100000;

class InventoryComponentManager : public Observer
{
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	ServerComponentOrchestrator& componentOrchestrator;
	ServerSocketManager& socketManager;
	std::map<int, int> idIndexMap;
	InventoryComponent inventoryComponents[MAX_INVENTORYCOMPONENTS_SIZE];
	int inventoryComponentIndex{ 0 };
public:
	InventoryComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager);
	InventoryComponent& CreateInventoryComponent(const int gameObjectId);
	void DeleteInventoryComponent(const int inventoryComponentId);
	InventoryComponent& GetInventoryComponentById(const int inventoryComponentId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
	~InventoryComponentManager();
};