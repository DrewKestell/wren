#include "stdafx.h"
#include "Repository.h"

Repository::Repository(const char* dbName)
	: dbName{ dbName }
{
}

sqlite3* Repository::GetConnection()
{
    sqlite3* dbConnection;
    if (sqlite3_open(dbName, &dbConnection) != SQLITE_OK)
        throw std::exception("Failed to open database.");

    return dbConnection;
}

sqlite3_stmt* Repository::PrepareStatement(sqlite3* dbConnection, std::span<const char> query)
{
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(dbConnection, query.data(), -1, &statement, nullptr) != SQLITE_OK)
    {
		PrintLastError(dbConnection);
        sqlite3_finalize(statement);
        throw std::exception("Failed to prepare SQLite statement.");
    }

    return statement;
}

void Repository::PrintLastError(sqlite3* dbConnection)
{
	std::cout << sqlite3_errmsg(dbConnection) << std::endl;
}