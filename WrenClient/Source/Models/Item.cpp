#include "stdafx.h"
#include "Item.h"

Item::Item(const int id, const char* name, const int spriteId, const bool stackable)
	: id{ id },
	name{ name },
	spriteId{ spriteId },
	stackable{ stackable }
{
}

const int Item::GetId() const { return id; }
const std::string& Item::GetName() const { return name; }
const int Item::GetSpriteId() const { return spriteId; }
const bool Item::GetStackable() const { return stackable; }
