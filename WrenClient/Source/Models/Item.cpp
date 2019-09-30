#include "stdafx.h"
#include "Item.h"

Item::Item(const int id, const char* name, const int spriteId, const int graySpriteId, const bool stackable)
	: id{ id },
	name{ name },
	spriteId{ spriteId },
	graySpriteId{ graySpriteId },
	stackable{ stackable }
{
}

const int Item::GetId() const { return id; }
const std::string& Item::GetName() const { return name; }
const int Item::GetSpriteId() const { return spriteId; }
const int Item::GetGraySpriteId() const { return graySpriteId; }
const bool Item::GetStackable() const { return stackable; }
