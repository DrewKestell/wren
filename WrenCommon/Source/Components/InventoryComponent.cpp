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

const bool InventoryComponent::MoveItem(const int sourceSlot, const int destinationSlot)
{
	if (itemIds.at(sourceSlot) < 0)
		return false;

	const auto tmpItemId = itemIds.at(destinationSlot);
	itemIds.at(destinationSlot) = itemIds.at(sourceSlot);
	itemIds.at(sourceSlot) = tmpItemId;
	
	return true;
}
