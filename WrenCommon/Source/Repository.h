#pragma once

#include <Span.h>

static constexpr auto FAILED_TO_PREPARE = "Failed to prepare SQLite statement.";
static constexpr auto FAILED_TO_EXECUTE = "Failed to execute statement.";

class Repository
{
	const char* dbName;
protected:
	sqlite3* GetConnection();
	sqlite3_stmt* PrepareStatement(sqlite3* dbConnection, std::span<const char> query);
	void PrintLastError(sqlite3* dbConnection);
public:
	Repository(const char* dbName);
};
