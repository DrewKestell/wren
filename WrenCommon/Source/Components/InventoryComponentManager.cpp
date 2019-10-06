#include "stdafx.h"
#include "InventoryComponentManager.h"
#include "EventHandling/Events/NpcDeathEvent.h"
#include <EventHandling/Events/LootItemSuccessEvent.h>

InventoryComponentManager::InventoryComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: ComponentManager(eventHandler, objectManager)
{
}

InventoryComponent& InventoryComponentManager::CreateInventoryComponent(const int gameObjectId)
{
	InventoryComponent& inventoryComponent = CreateComponent(gameObjectId);

	// additional initialization here

	return inventoryComponent;
}

const bool InventoryComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;

			ComponentManager::HandleEvent(event);

			break;
		}
		case EventType::NpcDeath:
		{
			const auto derivedEvent = (NpcDeathEvent*)event;

			const auto lootItemIds = derivedEvent->itemIds;
			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			InventoryComponent& inventoryComponent = GetComponentById(gameObject.inventoryComponentId);
			for (auto i = 0; i < lootItemIds.size(); i++)
				inventoryComponent.itemIds.at(i) = lootItemIds.at(i);

			break;
		}
		case EventType::LootItemSuccess:
		{
			const auto derivedEvent = (LootItemSuccessEvent*)event;

			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			InventoryComponent& inventoryComponent = GetComponentById(gameObject.inventoryComponentId);
			inventoryComponent.itemIds.at(derivedEvent->slot) = -1;

			break;
		}
	}
	return false;
}

void InventoryComponentManager::Update()
{
	
}