#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account
{
public:
    Account(const int id, const std::string& accountName, const std::string& password)
    {
        m_id = id;
        m_accountName = accountName;
        m_password = password;
    }
    int GetId() { return m_id; }
    std::string& GetAccountName() { return m_accountName; }
    std::string& GetPassword() { return m_password; }
private:
    int m_id;
    std::string m_accountName;
    std::string m_password;
};

#endif