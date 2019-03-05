#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <sqlite3.h>
#include "Account.h"

using namespace std;

class Repository
{
public:
    bool AccountExists(const string& accountName);
    void CreateAccount(const string& accountName, const string& password);
    Account* GetAccount(const string& accountName);
private:
    sqlite3* GetConnection();
    sqlite3_stmt* PrepareStatement(sqlite3* dbConnection, const char *query);
};

#endif