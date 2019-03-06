#include "SocketManager.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Account.h"
#include "Player.h"
#include "Repository.h"
#include "windows.h"

int main()
{
    Repository repository;
    SocketManager socketManager{ repository };
   
    socketManager.InitializeSockets();

    while (true)
    {
        socketManager.TryRecieveMessage();
        socketManager.HandleTimeout();
        Sleep(500);
    }
    
    socketManager.CloseSockets();    

    return 0;
}