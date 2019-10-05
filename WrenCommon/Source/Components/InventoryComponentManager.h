#pragma once

#include <Constants.h>
#include <EventHandling/Observer.h>
#include "InventoryComponent.h"
#include "ObjectManager.h"
#include <Components/ComponentManager.h>

class InventoryComponentManager : public ComponentManager<InventoryComponent, 100000>
{
public:
	InventoryComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	InventoryComponent& CreateInventoryComponent(const int gameObjectId);
	virtual const bool HandleEvent(const Event* const event);
	void Update();
};