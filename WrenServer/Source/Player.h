#pragma once

class Player
{
	const int accountId;
	const std::string token;
    const std::string ipAndPort;
	DWORD lastHeartbeat;
	std::string characterName;
	int updateCounter{ 0 };
    
public:
    Player(
		const int accountId,
		const std::string& token,
		const std::string& ipAndPort,
		const DWORD lastHeartbeat)
		: accountId{ accountId },
		  token{ token },
		  ipAndPort { ipAndPort },
		  lastHeartbeat { lastHeartbeat }
    {
	}
	const int GetAccountId() const { return accountId; }
	const std::string& GetToken() const { return token; }
    const std::string& GetCharacterName() const { return characterName; }
	void SetCharacterName(const std::string characterName) { this->characterName = characterName; }
    const std::string& GetIPAndPort() const { return ipAndPort; }
    const DWORD GetLastHeartbeat() const { return lastHeartbeat; }
    void SetLastHeartbeat(const DWORD heartbeat) { lastHeartbeat = heartbeat; }
	void IncrementUpdateCounter() { updateCounter++; };
	const int GetUpdateCounter() const { return updateCounter; }
};
