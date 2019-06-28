#pragma once

#include "Event.h"

class SendChatMessage : public Event
{
public:
	SendChatMessage(const std::string* message)
		: Event(EventType::SendChatMessage),
		  message{ message }
	{
	}
	~SendChatMessage() { delete message; }
	const std::string* message;
};