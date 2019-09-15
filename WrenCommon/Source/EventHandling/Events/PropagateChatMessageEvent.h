#pragma once

#include "Event.h"

class PropagateChatMessageEvent : public Event
{
public:
	PropagateChatMessageEvent(std::string* senderName, std::string* message)
		: Event(EventType::PropagateChatMessage),
		  senderName{ senderName },
		  message{ message }
	{
	}
	~PropagateChatMessageEvent() { delete message; }
	std::string* senderName;
	std::string* message;
};