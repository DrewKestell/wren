#pragma once

#include <Models/Skill.h>
#include "Models/Account.h"
#include "Models/Character.h"

class Repository
{
	sqlite3* GetConnection();
	sqlite3_stmt* PrepareStatement(sqlite3* dbConnection, const char *query);
public:
    bool AccountExists(const std::string& accountName);
	bool CharacterExists(const std::string& characterName);
    void CreateAccount(const std::string& accountName, const std::string& password);
	void CreateCharacter(const std::string& characterName, const int accountId);
    Account* GetAccount(const std::string& accountName);
    std::vector<std::string>* ListCharacters(const int accountId);
	void DeleteCharacter(const std::string& characterName);
	Character* GetCharacter(const std::string& characterName);
	std::vector<Skill> ListSkills(const int characterId);
};
