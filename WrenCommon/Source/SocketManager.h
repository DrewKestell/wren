#pragma once

#include <OpCodes.h>

class SocketManager
{
	bool TryRecieveMessage();

protected:
	int sockaddr_in_len{ sizeof(sockaddr_in) };
	sockaddr_in local;
	SOCKET sock;
	sockaddr_in from;
	std::map<OpCode, std::function<void(std::vector<std::string>& args)>> messageHandlers;

	SocketManager(const int localPort = 0);
	virtual void InitializeMessageHandlers() = 0;
	void SendPacket(const sockaddr_in& to, const OpCode opCode, const std::vector<std::string>& args);

public:
	void ProcessPackets();
	void CloseSockets();

};