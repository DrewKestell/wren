#include "windows.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    std::string Name;
    std::string IPAndPort;
    DWORD LastHeartbeat;
};

#endif