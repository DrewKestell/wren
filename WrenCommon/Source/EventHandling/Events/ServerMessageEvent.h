#pragma once

#include "Event.h"

class ServerMessageEvent : public Event
{
public:
	ServerMessageEvent(const std::string& message, const std::string& type)
		: Event(EventType::ServerMessage),
		message{ message },
		type{ message }
	{
	}
	const std::string message;
	const std::string type;
};