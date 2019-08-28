#include "stdafx.h"
#include "ClientRepository.h"

const auto LIST_NPCS_QUERY = "SELECT id, name, model_id, texture_id, speed FROM Npcs;";

std::vector<Npc*>* ClientRepository::ListNpcs()
{
	auto dbConnection = GetConnection();

	auto statement = PrepareStatement(dbConnection, LIST_NPCS_QUERY);

	std::vector<Npc*>* npcs = new std::vector<Npc*>;
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
			const unsigned char *name = sqlite3_column_text(statement, 1);
			const int modelId = sqlite3_column_int(statement, 2);
			const int textureId = sqlite3_column_int(statement, 3);
			const auto speed = (float)sqlite3_column_double(statement, 4);
			npcs->push_back(new Npc(id, new std::string(reinterpret_cast<const char*>(name)), modelId, textureId, speed));
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