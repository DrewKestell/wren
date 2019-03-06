#include <string>
#include <algorithm>
#include "windows.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    std::string name;
    std::string ipAndPort;
    DWORD lastHeartbeat;
public:
    Player(const std::string& name, const std::string& ipAndPort, const DWORD lastHeartbeat) :
        name{ name }, 
        ipAndPort { ipAndPort },
        lastHeartbeat { lastHeartbeat }
    {}
    const std::string& GetName() { return name; }
    const std::string& GetIPAndPort() { return ipAndPort; }
    const DWORD GetLastHeartbeat() { return lastHeartbeat; }
    void SetLastHeartbeat(DWORD heartbeat) { lastHeartbeat = heartbeat; }
};

#endif