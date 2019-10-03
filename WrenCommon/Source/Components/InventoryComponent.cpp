#include "stdafx.h"
#include "InventoryComponent.h"

void InventoryComponent::Initialize(const int id, const int gameObjectId)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
}

const int InventoryComponent::GetId() const { return id; }
const int InventoryComponent::GetGameObjectId() const { return gameObjectId; }

const bool InventoryComponent::IsInventoryFull() const
{
	return itemIds.size() < INVENTORY_SIZE;
}

const int InventoryComponent::AddItem(const int itemId)
{
	for (auto i = 0; i < INVENTORY_SIZE; i++)
	{
		if (itemIds.at(i) == -1)
		{
			itemIds.at(i) = itemId;
			return i;
		}
	}

	return -1;
}

const bool InventoryComponent::AddItemAtSlot(const int itemId, const int slot)
{
	if (itemIds.at(slot) == -1)
	{
		itemIds.at(slot) = itemId;
		return true;
	}

	return false;
}