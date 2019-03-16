#include <string>
#include <algorithm>
#include "windows.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
	int accountId;
	std::string token;
    std::string characterName;
    std::string ipAndPort;
    DWORD lastHeartbeat;
public:
    Player(
		const int accountId,
		const std::string& token,
		const std::string& ipAndPort,
		const DWORD lastHeartbeat) :
			token{ token },
			ipAndPort { ipAndPort },
			lastHeartbeat { lastHeartbeat }
    {}
	const int GetAccountId() { return accountId; }
	const std::string& GetToken() { return token; }
    const std::string& GetCharacterName() { return characterName; }
	void SetCharacterName(const std::string characterName) { this->characterName = characterName; }
    const std::string& GetIPAndPort() { return ipAndPort; }
    const DWORD GetLastHeartbeat() { return lastHeartbeat; }
    void SetLastHeartbeat(const DWORD heartbeat) { lastHeartbeat = heartbeat; }
};

#endif