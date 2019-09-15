#pragma once

#include <string>

class Account
{
public:
    Account(const int id, const std::string& accountName, const std::string& password)
		: id{ id },
		  accountName{ accountName },
		  password{ password }
    {
    }
    const int GetId() const { return id; }
    const std::string& GetAccountName() const { return accountName; }
    const std::string& GetPassword() const { return password; }
private:
    const int id;
    const std::string accountName;
    const std::string password;
};
