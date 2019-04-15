#include "stdafx.h"
#include "Repository.h"

constexpr auto DB_NAME = "Wren.db";

constexpr auto FAILED_TO_OPEN = "Failed to open database.";
constexpr auto FAILED_TO_PREPARE = "Failed to prepare SQLite statement.";
constexpr auto FAILED_TO_EXECUTE = "Failed to execute statement.";

constexpr auto ACCOUNT_EXISTS_QUERY = "SELECT id FROM Accounts WHERE account_name = '%s' LIMIT 1;";
constexpr auto CHARACTER_EXISTS_QUERY = "SELECT id FROM Characters WHERE character_name = '%s' LIMIT 1;";
constexpr auto CREATE_ACCOUNT_QUERY = "INSERT INTO Accounts (account_name, hashed_password) VALUES('%s', '%s');";
constexpr auto CREATE_CHARACTER_QUERY = "INSERT INTO Characters (character_name, account_id) VALUES('%s', '%d');";
constexpr auto GET_ACCOUNT_QUERY = "SELECT * FROM Accounts WHERE account_name = '%s' LIMIT 1;";
constexpr auto LIST_CHARACTERS_QUERY = "SELECT * FROM Characters WHERE account_id = '%d';";
constexpr auto DELETE_CHARACTER_QUERY = "DELETE FROM Characters WHERE character_name = '%s';";

bool Repository::AccountExists(const std::string& accountName)
{
    const auto dbConnection = GetConnection();

    char query[100];
    sprintf_s(query, ACCOUNT_EXISTS_QUERY, accountName.c_str());
        
    const auto statement = PrepareStatement(dbConnection, query);

    const auto result = sqlite3_step(statement);
    if (result == SQLITE_ROW)
    {
        sqlite3_finalize(statement);
        return true;
    }
    else if (result == SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return false;
    }
    else
    {
        sqlite3_finalize(statement);
        throw std::exception(FAILED_TO_EXECUTE);
    }
}

bool Repository::CharacterExists(const std::string& characterName)
{
	const auto dbConnection = GetConnection();

	char query[100];
	sprintf_s(query, CHARACTER_EXISTS_QUERY, characterName.c_str());

	const auto statement = PrepareStatement(dbConnection, query);

	const auto result = sqlite3_step(statement);
	if (result == SQLITE_ROW)
	{
		sqlite3_finalize(statement);
		return true;
	}
	else if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return false;
	}
	else
	{
		sqlite3_finalize(statement);
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

void Repository::CreateAccount(const std::string& accountName, const std::string& password)
{
    const auto dbConnection = GetConnection();

    char query[300];
    sprintf_s(query, CREATE_ACCOUNT_QUERY, accountName.c_str(), password.c_str());

	const auto statement = PrepareStatement(dbConnection, query);

    if (sqlite3_step(statement) != SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        throw std::exception(FAILED_TO_EXECUTE);
    }
}

void Repository::CreateCharacter(const std::string& characterName, const int accountId)
{
	const auto dbConnection = GetConnection();

	char query[300];
	sprintf_s(query, CREATE_CHARACTER_QUERY, characterName.c_str(), accountId);

	const auto statement = PrepareStatement(dbConnection, query);

	if (sqlite3_step(statement) != SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

Account* Repository::GetAccount(const std::string& accountName)
{
    auto dbConnection = GetConnection();
                
    char query[100];
    sprintf_s(query, GET_ACCOUNT_QUERY, accountName.c_str());

    auto statement = PrepareStatement(dbConnection, query);

    const auto result = sqlite3_step(statement);
    if (result == SQLITE_ROW)
    {
        const int id = sqlite3_column_int(statement, 0);
        const unsigned char *hashedPassword = sqlite3_column_text(statement, 2);
        auto account = new Account(id, accountName, std::string(reinterpret_cast<const char*>(hashedPassword)));
        sqlite3_finalize(statement);
        return account;
    }
    else if (result == SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return nullptr;
    }
    else
    {
        sqlite3_finalize(statement);
        throw std::exception(FAILED_TO_EXECUTE);
    }
}

sqlite3* Repository::GetConnection()
{
    sqlite3* dbConnection;
    if (sqlite3_open(DB_NAME, &dbConnection) != SQLITE_OK)
        throw std::exception(FAILED_TO_OPEN);
    return dbConnection;
}

sqlite3_stmt* Repository::PrepareStatement(sqlite3* dbConnection, const char *query)
{
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(dbConnection, query, -1, &statement, NULL) != SQLITE_OK)
    {
        sqlite3_finalize(statement);
        throw std::exception(FAILED_TO_PREPARE);
    }
    return statement;
}

std::vector<std::string>* Repository::ListCharacters(const int accountId)
{
    auto dbConnection = GetConnection();

    char query[100];
    sprintf_s(query, LIST_CHARACTERS_QUERY, accountId);

    auto statement = PrepareStatement(dbConnection, query);

    std::vector<std::string>* characters = new std::vector<std::string>;
    auto result = sqlite3_step(statement);
    if (result == SQLITE_DONE)
    {
        sqlite3_finalize(statement);
        return characters;
    }        
    else if (result == SQLITE_ROW)
    {
        while (result == SQLITE_ROW)
        {
            const unsigned char *characterName = sqlite3_column_text(statement, 1);
            characters->push_back(std::string(reinterpret_cast<const char*>(characterName)));
            result = sqlite3_step(statement);
        }
        sqlite3_finalize(statement);
        return characters;
    }
    else
    {
        sqlite3_finalize(statement);
        throw std::exception(FAILED_TO_EXECUTE);
    }
}

void Repository::DeleteCharacter(const std::string& characterName)
{
	auto dbConnection = GetConnection();

	char query[100];
	sprintf_s(query, DELETE_CHARACTER_QUERY, characterName.c_str());

	auto statement = PrepareStatement(dbConnection, query);
	if (sqlite3_step(statement) == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return;
	}
	else
	{
		sqlite3_finalize(statement);
		throw std::exception(FAILED_TO_EXECUTE);
	}
}