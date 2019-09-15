#pragma once

#include <OpCodes.h>

class SocketManager
{
	bool TryRecieveMessage();

protected:
	SOCKET sock;
	sockaddr_in from;
	int fromlen;
	std::map<OpCode, std::function<void(std::vector<std::string>& args)>> messageHandlers;

	virtual void InitializeMessageHandlers() = 0;

public:
	void ProcessPackets();
	void CloseSockets();

};