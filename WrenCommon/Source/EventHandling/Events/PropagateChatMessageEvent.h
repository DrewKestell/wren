#pragma once

#include "Event.h"

class PropagateChatMessageEvent : public Event
{
public:
	PropagateChatMessageEvent(const std::string& senderName, const std::string& message)
		: Event(EventType::PropagateChatMessage),
		  senderName{ senderName },
		  message{ message }
	{
	}
	const std::string senderName;
	const std::string message;
};