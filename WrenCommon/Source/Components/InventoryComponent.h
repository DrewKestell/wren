#pragma once

#include "GameObject.h"

class InventoryComponent
{
	int id{ 0 };
	int gameObjectId{ 0 };

	void Initialize(const int id, const int gameObjectId);

	friend class InventoryComponentManager;
public:
	std::vector<int> itemIds = std::vector<int>(INVENTORY_SIZE, -1);

	const int GetId() const;
	const int GetGameObjectId() const;
	
	const bool IsInventoryFull() const;
	const int AddItem(const int itemId);
	const bool AddItemAtSlot(const int itemId, const int slot);
};