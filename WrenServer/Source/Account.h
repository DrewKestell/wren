#pragma once

#include <string>

class Account
{
public:
    Account(const int id, const std::string& accountName, const std::string& password)
		: m_id{ id },
		  m_accountName{ accountName },
		  m_password{ password }
    {
    }
    int GetId() const { return m_id; }
    const std::string& GetAccountName() const { return m_accountName; }
    const std::string& GetPassword() const { return m_password; }
private:
    const int m_id;
    const std::string m_accountName;
    const std::string m_password;
};
