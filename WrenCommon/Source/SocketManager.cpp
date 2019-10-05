#include "stdafx.h"
#include "SocketManager.h"
#include "Constants.h"

SocketManager::SocketManager(EventHandler& eventHandler, const int localPort)
	: eventHandler{ eventHandler }
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		throw std::exception("Failed to initialize sockets.");

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	if (localPort > 0)
		local.sin_port = htons(localPort);
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == INVALID_SOCKET)
	{
		const auto error = WSAGetLastError();
		throw std::exception("Failed to create new socket. Error: " + error);
	}

	bind(sock, (sockaddr*)& local, sockaddr_in_len);
	DWORD nonBlocking = 1;
	ioctlsocket(sock, FIONBIO, &nonBlocking);
}

bool SocketManager::TryRecieveMessage()
{
	char buffer[1024];
	ZeroMemory(buffer, sizeof(buffer));
	const auto result = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)& from, &sockaddr_in_len);
	if (result == SOCKET_ERROR)
	{
		const auto errorCode = WSAGetLastError();

		if (errorCode == WSAEWOULDBLOCK)
			return false;

		throw std::exception("WrenServer SocketManager error receiving packet. Error code: " + errorCode);
	}
	else
	{
		int offset{ 0 };

		// if the checksum is wrong, ignore the packet
		int checksum{ 0 };
		memcpy(&checksum, buffer, sizeof(OpCode));
		offset += sizeof(OpCode);
		if (checksum != static_cast<int>(OpCode::Checksum))
			return true;

		OpCode opCode{ };
		memcpy(&opCode, buffer + offset, sizeof(OpCode));
		offset += sizeof(OpCode);

		std::vector<std::string> args;
		const auto bufferLength = strlen(buffer + offset);
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

void SocketManager::SendPacket(const sockaddr_in& to, const OpCode opCode, const std::vector<std::string>& args)
{
	char buffer[PACKET_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int offset{ 0 };

	memcpy(buffer, &CHECKSUM, sizeof(OpCode));
	offset += int{ sizeof(OpCode) };

	memcpy(buffer + offset, &opCode, sizeof(OpCode));
	offset += int{ sizeof(OpCode) };

	std::string packet{ "" };
	for (auto i = 0; i < args.size(); i++)
		packet += args[i] + "|";

	strcpy_s(buffer + offset, packet.length() + 1, packet.c_str());
	const auto sentBytes = sendto(sock, buffer, sizeof(buffer), 0, (sockaddr*)& to, sockaddr_in_len);
	if (sentBytes != sizeof(buffer))
		throw std::exception("Failed to send packet.");
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
