#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <sqlite3.h>
#include "Account.h"

class Repository
{
public:
    bool AccountExists(const std::string& accountName);
    void CreateAccount(const std::string& accountName, const std::string& password);
    Account* GetAccount(const std::string& accountName);
private:
    sqlite3* GetConnection();
    sqlite3_stmt* PrepareStatement(sqlite3* dbConnection, const char *query);
};

#endif