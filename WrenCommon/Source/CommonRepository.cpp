#include "stdafx.h"
#include "CommonRepository.h"

const auto LIST_STATIC_OBJECTS_QUERY = "SELECT id, name, model_id, texture_id, position_x, position_y, position_z FROM StaticObjects;";

std::vector<StaticObject*> CommonRepository::ListStaticObjects()
{
	auto dbConnection = GetConnection();

	auto statement = PrepareStatement(dbConnection, LIST_STATIC_OBJECTS_QUERY);

	std::vector<StaticObject*> staticObjects;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return staticObjects;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			const unsigned int id = sqlite3_column_int(statement, 0);
			const auto name = new std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
			const unsigned int modelId = sqlite3_column_int(statement, 2);
			const unsigned int textureId = sqlite3_column_int(statement, 3);
			const auto positionX = (float)sqlite3_column_double(statement, 4);
			const auto positionY = (float)sqlite3_column_double(statement, 5);
			const auto positionZ = (float)sqlite3_column_double(statement, 6);
								   
			staticObjects.push_back(new StaticObject{ id, name, modelId, textureId, XMFLOAT3{ positionX, positionY, positionZ } });
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return staticObjects;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}