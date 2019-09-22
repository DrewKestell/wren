#include "stdafx.h"
#include "ClientRepository.h"

constexpr char LIST_NPCS_QUERY[] = "SELECT id, name, model_id, texture_id, speed FROM Npcs;";

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
			const int id = sqlite3_column_int(statement, 0);
			const auto name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
			const int modelId = sqlite3_column_int(statement, 2);
			const int textureId = sqlite3_column_int(statement, 3);
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