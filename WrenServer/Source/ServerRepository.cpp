#include "stdafx.h"
#include "ServerRepository.h"

const auto ACCOUNT_EXISTS_QUERY = "SELECT id FROM Accounts WHERE account_name = '%s' LIMIT 1;";
const auto CHARACTER_EXISTS_QUERY = "SELECT id FROM Characters WHERE character_name = '%s' LIMIT 1;";
const auto CREATE_ACCOUNT_QUERY = "INSERT INTO Accounts (account_name, hashed_password) VALUES('%s', '%s');";
const auto CREATE_CHARACTER_QUERY = "INSERT INTO Characters (character_name, account_id, position_x, position_y, position_z) VALUES('%s', '%d', 0.0, 0.0, 0.0);";
const auto GET_ACCOUNT_QUERY = "SELECT * FROM Accounts WHERE account_name = '%s' LIMIT 1;";
const auto LIST_CHARACTERS_QUERY = "SELECT * FROM Characters WHERE account_id = '%d';";
const auto DELETE_CHARACTER_QUERY = "DELETE FROM Characters WHERE character_name = '%s';";
const auto GET_CHARACTER_QUERY = "SELECT * FROM Characters WHERE character_name = '%s' LIMIT 1;";
const auto LIST_CHARACTER_SKILLS_QUERY = "SELECT Skills.id, Skills.name, CharacterSkills.value FROM CharacterSkills INNER JOIN Skills on Skills.id = CharacterSkills.skill_id WHERE CharacterSkills.character_id = '%d';";
const auto LIST_CHARACTER_ABILITIES_QUERY = "SELECT Abilities.id, Abilities.name, Abilities.sprite_id, Abilities.toggled FROM CharacterAbilities INNER JOIN Abilities on Abilities.id = CharacterAbilities.ability_id WHERE CharacterAbilities.character_id = '%d';";
const auto LIST_ABILITIES_QUERY = "SELECT id, name, sprite_id, toggled, targeted FROM Abilities;";

const bool ServerRepository::AccountExists(const std::string& accountName)
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
		PrintLastError(dbConnection);
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

const bool ServerRepository::CharacterExists(const std::string& characterName)
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
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

void ServerRepository::CreateAccount(const std::string& accountName, const std::string& password)
{
	const auto dbConnection = GetConnection();

	char query[300];
	sprintf_s(query, CREATE_ACCOUNT_QUERY, accountName.c_str(), password.c_str());

	const auto statement = PrepareStatement(dbConnection, query);

	if (sqlite3_step(statement) != SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

void ServerRepository::CreateCharacter(const std::string& characterName, const int accountId)
{
	const auto dbConnection = GetConnection();

	char query[300];
	sprintf_s(query, CREATE_CHARACTER_QUERY, characterName.c_str(), accountId);

	const auto statement = PrepareStatement(dbConnection, query);

	if (sqlite3_step(statement) != SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

Account* ServerRepository::GetAccount(const std::string& accountName)
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
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

std::vector<std::string> ServerRepository::ListCharacters(const int accountId)
{
	auto dbConnection = GetConnection();

	char query[100];
	sprintf_s(query, LIST_CHARACTERS_QUERY, accountId);

	auto statement = PrepareStatement(dbConnection, query);

	std::vector<std::string> characters;
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
			characters.push_back(std::string(reinterpret_cast<const char*>(characterName)));
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return characters;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

void ServerRepository::DeleteCharacter(const std::string& characterName)
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
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

Character* ServerRepository::GetCharacter(const std::string& characterName)
{
	auto dbConnection = GetConnection();

	char query[100];
	sprintf_s(query, GET_CHARACTER_QUERY, characterName.c_str());

	auto statement = PrepareStatement(dbConnection, query);
	if (sqlite3_step(statement) == SQLITE_ROW)
	{
		const int id = sqlite3_column_int(statement, 0);
		const unsigned char *characterName = sqlite3_column_text(statement, 1);
		const int accountId = sqlite3_column_int(statement, 2);
		const double positionX = sqlite3_column_double(statement, 3);
		const double positionY = sqlite3_column_double(statement, 4);
		const double positionZ = sqlite3_column_double(statement, 5);
		const int modelId = sqlite3_column_int(statement, 6);
		const int textureId = sqlite3_column_int(statement, 7);
		auto character = new Character(id, std::string(reinterpret_cast<const char*>(characterName)), accountId, XMFLOAT3{ (float)positionX, (float)positionY, (float)positionZ }, modelId, textureId);

		sqlite3_finalize(statement);
		return character;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

std::vector<WrenCommon::Skill> ServerRepository::ListCharacterSkills(const int characterId)
{
	auto dbConnection = GetConnection();

	char query[200];
	sprintf_s(query, LIST_CHARACTER_SKILLS_QUERY, characterId);

	auto statement = PrepareStatement(dbConnection, query);

	std::vector<WrenCommon::Skill> skills;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return skills;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			const auto skillId = sqlite3_column_int(statement, 0);
			const unsigned char *skillName = sqlite3_column_text(statement, 1);
			const auto value = sqlite3_column_int(statement, 2);
			skills.push_back(WrenCommon::Skill{ skillId, std::string(reinterpret_cast<const char*>(skillName)), value });
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return skills;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

std::vector<Ability> ServerRepository::ListCharacterAbilities(const int characterId)
{
	auto dbConnection = GetConnection();

	char query[300];
	sprintf_s(query, LIST_CHARACTER_ABILITIES_QUERY, characterId);

	auto statement = PrepareStatement(dbConnection, query);

	std::vector<Ability> abilities;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return abilities;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			const auto abilityId = sqlite3_column_int(statement, 0);
			const unsigned char *name = sqlite3_column_text(statement, 1);
			const auto spriteId = sqlite3_column_int(statement, 2);
			const auto toggled = sqlite3_column_int(statement, 3) == 1;
			const auto targeted = sqlite3_column_int(statement, 4) == 1;
			abilities.push_back(Ability(abilityId, std::string(reinterpret_cast<const char*>(name)), spriteId, toggled, targeted));
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return abilities;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}

std::vector<Ability> ServerRepository::ListAbilities()
{
	auto dbConnection = GetConnection();

	auto statement = PrepareStatement(dbConnection, LIST_ABILITIES_QUERY);

	std::vector<Ability> abilities;
	auto result = sqlite3_step(statement);
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(statement);
		return abilities;
	}
	else if (result == SQLITE_ROW)
	{
		while (result == SQLITE_ROW)
		{
			const auto abilityId = sqlite3_column_int(statement, 0);
			const unsigned char *name = sqlite3_column_text(statement, 1);
			const auto spriteId = sqlite3_column_int(statement, 2);
			const auto toggled = sqlite3_column_int(statement, 3) == 1;
			const auto targeted = sqlite3_column_int(statement, 4) == 1;
			abilities.push_back(Ability(abilityId, std::string(reinterpret_cast<const char*>(name)), spriteId, toggled, targeted));
			result = sqlite3_step(statement);
		}
		sqlite3_finalize(statement);
		return abilities;
	}
	else
	{
		sqlite3_finalize(statement);
		std::cout << sqlite3_errmsg(dbConnection) << std::endl;
		throw std::exception(FAILED_TO_EXECUTE);
	}
}