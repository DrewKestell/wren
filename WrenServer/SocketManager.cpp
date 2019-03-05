#include <vector>
#include "SocketManager.h"
#include "Player.h"

using namespace std;

constexpr auto TIMEOUT_DURATION = 30000; // 30000ms == 30s

void SocketManager::HandleTimeout()
{
    vector<Player>::const_iterator it = PLAYERS.begin();
    for_each(PLAYERS.begin(), PLAYERS.end(), [&it](Player player) {
        if (GetTickCount() > player.LastHeartbeat + TIMEOUT_DURATION)
        {
            std::cout << player.Name << " timed out." << "\n";
            PLAYERS.erase(it);
        }
    });
}