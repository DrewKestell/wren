#include "stdafx.h"
#include "InventoryComponentManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"
#include "EventHandling/Events/NpcDeathEvent.h"

InventoryComponentManager::InventoryComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager }
{
	eventHandler.Subscribe(*this);
}

InventoryComponent& InventoryComponentManager::CreateInventoryComponent(const int gameObjectId)
{
	if (inventoryComponentIndex == MAX_INVENTORYCOMPONENTS_SIZE)
		throw std::exception("Max InventoryComponents exceeded!");

	inventoryComponents[inventoryComponentIndex].Initialize(inventoryComponentIndex, gameObjectId);
	idIndexMap[inventoryComponentIndex] = inventoryComponentIndex;

	InventoryComponent& inventoryComponent = inventoryComponents[inventoryComponentIndex++];

	// initialize component
	for (auto i = 0; i < INVENTORY_SIZE; i++)
		inventoryComponent.itemIds.at(i) = -1;

	return inventoryComponent;
}

void InventoryComponentManager::DeleteInventoryComponent(const int inventoryComponentId)
{
	// first copy the last InventoryComponent into the index that was deleted
	const auto inventoryComponentToDeleteIndex = idIndexMap[inventoryComponentId];
	const auto lastInventoryComponentIndex = --inventoryComponentIndex;
	memcpy(&inventoryComponents[inventoryComponentToDeleteIndex], &inventoryComponents[lastInventoryComponentIndex], sizeof(InventoryComponent));

	// then update the index of the moved InventoryComponent
	const auto lastInventoryComponentId = inventoryComponents[inventoryComponentToDeleteIndex].id;
	idIndexMap[lastInventoryComponentId] = inventoryComponentToDeleteIndex;
}

InventoryComponent& InventoryComponentManager::GetInventoryComponentById(const int inventoryComponentId)
{
	const auto index = idIndexMap[inventoryComponentId];
	return inventoryComponents[index];
}

const bool InventoryComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;

			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteInventoryComponent(gameObject.inventoryComponentId);

			break;
		}
		case EventType::NpcDeath:
		{
			const auto derivedEvent = (NpcDeathEvent*)event;

			const auto lootItemIds = derivedEvent->itemIds;
			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			InventoryComponent& inventoryComponent = GetInventoryComponentById(gameObject.inventoryComponentId);
			for (auto i = 0; i < lootItemIds.size(); i++)
				inventoryComponent.itemIds.at(i) = lootItemIds.at(i);

			break;
		}
	}
	return false;
}

void InventoryComponentManager::Update()
{
	
}

InventoryComponentManager::~InventoryComponentManager()
{
	eventHandler.Unsubscribe(*this);
}