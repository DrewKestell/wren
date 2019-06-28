#pragma once

#include "Event.h"

class PropagateChatMessage : public Event
{
public:
	PropagateChatMessage(std::string* senderName, std::string* message)
		: Event(EventType::PropagateChatMessage),
		  senderName{ senderName },
		  message{ message }
	{
	}
	~PropagateChatMessage() { delete message; }
	std::string* senderName;
	std::string* message;
};