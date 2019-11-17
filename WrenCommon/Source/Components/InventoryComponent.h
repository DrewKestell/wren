#pragma once

#include <Components/Component.h>

class InventoryComponent : public Component
{

	friend class InventoryComponentManager;
public:
	std::vector<int> itemIds = std::vector<int>(INVENTORY_SIZE, -1);

	const bool IsInventoryFull() const;
	const int AddItem(const int itemId);
	const bool MoveItem(const int sourceSlot, const int destinationSlot);
};
