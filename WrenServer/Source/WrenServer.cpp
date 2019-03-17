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
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 800, 0, 800, 800, TRUE);
    std::cout << "WrenServer initialized.\n";

    Repository repository;
    SocketManager socketManager{ repository };

    while (true)
    {
        socketManager.TryRecieveMessage();
        socketManager.HandleTimeout();
    }
    
    socketManager.CloseSockets();    

    return 0;
}