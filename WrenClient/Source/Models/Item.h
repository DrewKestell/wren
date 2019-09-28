#pragma once

class Item
{
	const int id;
	const std::string name;
	const int spriteId;
	const bool stackable;

public:
	Item(const int id, const char* name, const int spriteId, const bool stackable);

	const int GetId() const;
	const std::string& GetName() const;
	const int GetSpriteId() const;
	const bool GetStackable() const;
};
