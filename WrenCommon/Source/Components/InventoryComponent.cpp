#include "stdafx.h"
#include "InventoryComponent.h"

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