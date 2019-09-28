#include "stdafx.h"
#include "InventoryComponent.h"

void InventoryComponent::Initialize(const int id, const int gameObjectId)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
}

const int InventoryComponent::GetId() const { return id; }
const int InventoryComponent::GetGameObjectId() const { return gameObjectId; }