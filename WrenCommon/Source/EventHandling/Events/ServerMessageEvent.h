#pragma once

#include "Event.h"

class ServerMessageEvent : public Event
{
public:
	ServerMessageEvent(const std::string* message, const std::string* type)
		: Event(EventType::ServerMessage),
		message{ message },
		type{ type }
	{
	}
	~ServerMessageEvent() { delete message; delete type; }
	const std::string* message;
	const std::string* type;
};