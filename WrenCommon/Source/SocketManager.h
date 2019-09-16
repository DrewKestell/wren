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
	void SendPacket(sockaddr_in to, const OpCode opCode);
	void SendPacket(sockaddr_in to, const OpCode opCode, std::string*, const int argCount, const int accountId = -1, const std::string& token = "");

public:
	void ProcessPackets();
	void CloseSockets();

};