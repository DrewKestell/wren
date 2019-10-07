#include "stdafx.h"
#include "ClientRepository.h"

constexpr char LIST_NPCS_QUERY[] = "SELECT id, name, model_id, texture_id, speed FROM Npcs;";
constexpr char LIST_ITEMS_QUERY[] = "SELECT id, name, description, sprite_id, gray_sprite_id, stackable FROM Items;";

std::vector<std::unique_ptr<Npc>> ClientRepository::ListNpcs()
{
	auto dbConnection = GetConnection();
	auto statement = PrepareStatement(dbConnection, LIST_NPCS_QUERY);

	std::vector<std::unique_ptr<Npc>> npcs;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return npcs;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			const auto id = sqlite3_column_int(statement, 0);
			const auto name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
			const auto modelId = sqlite3_column_int(statement, 2);
			const auto textureId = sqlite3_column_int(statement, 3);
			const auto speed = static_cast<float>(sqlite3_column_double(statement, 4));
			npcs.push_back(std::make_unique<Npc>(id, name, modelId, textureId, speed));
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return npcs;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

std::vector<std::unique_ptr<Item>> ClientRepository::ListItems()
{
	auto dbConnection = GetConnection();
	auto statement = PrepareStatement(dbConnection, LIST_ITEMS_QUERY);

	std::vector<std::unique_ptr<Item>> items;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return items;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			auto i = 0;
			const auto id = sqlite3_column_int(statement, i++);
			const auto name = reinterpret_cast<const char*>(sqlite3_column_text(statement, i++));
			const auto description = reinterpret_cast<const char*>(sqlite3_column_text(statement, i++));
			const auto spriteId = sqlite3_column_int(statement, i++);
			const auto graySpriteId = sqlite3_column_int(statement, i++);
			const auto stackable = sqlite3_column_int(statement, i++) == 1;
			items.push_back(std::make_unique<Item>(id, name, description, spriteId, graySpriteId, stackable));
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return items;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}