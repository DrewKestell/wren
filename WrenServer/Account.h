#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

using namespace std;

class Account
{
public:
    Account(const int id, const string& accountName, const string& password)
    {
        m_id = id;
        m_accountName = accountName;
        m_password = password;
    }
    int GetId() { return m_id; }
    string& GetAccountName() { return m_accountName; }
    string& GetPassword() { return m_password; }
private:
    int m_id;
    string m_accountName;
    string m_password;
};

#endif