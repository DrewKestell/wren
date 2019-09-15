#include "stdafx.h"
#include "SocketManager.h"

bool SocketManager::TryRecieveMessage()
{
	char buffer[1024];
	ZeroMemory(buffer, sizeof(buffer));
	auto result = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)& from, &fromlen);
	if (result == SOCKET_ERROR)
	{
		auto errorCode = WSAGetLastError();

		if (errorCode == WSAEWOULDBLOCK)
			return false;

		throw new std::exception("WrenServer SocketManager error receiving packet. Error code: " + errorCode);
	}
	else
	{
		int offset{ 0 };

		// if the checksum is wrong, ignore the packet
		int checksum{ 0 };
		memcpy(&checksum, buffer, sizeof(OpCode));
		offset += sizeof(OpCode);
		if (checksum != (int)OpCode::Checksum)
			return true;

		OpCode opCode{ };
		memcpy(&opCode, buffer + offset, sizeof(OpCode));
		offset += sizeof(OpCode);

		std::vector<std::string> args;
		auto bufferLength = strlen(buffer + offset);
		if (bufferLength > 0)
		{
			std::string arg = "";
			for (unsigned int i = offset; i < offset + bufferLength; i++)
			{
				if (buffer[i] == '|')
				{
					args.push_back(arg);
					arg = "";
				}
				else
					arg += buffer[i];
			}
		}

		const auto fun = messageHandlers[opCode];
		if (fun)
		{
			fun(args);
			return true;
		}

		return false;
	}
}

void SocketManager::ProcessPackets()
{
	while (TryRecieveMessage()) {}
}

void SocketManager::CloseSockets()
{
	closesocket(sock);
	WSACleanup();
}