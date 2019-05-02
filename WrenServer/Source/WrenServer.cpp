#include "stdafx.h"
#include <ObjectManager.h>
#include "SocketManager.h"

ObjectManager g_objectManager;

int main()
{
    HWND consoleWindow = GetConsoleWindow();
    MoveWindow(consoleWindow, 810, 0, 800, 800, TRUE);
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