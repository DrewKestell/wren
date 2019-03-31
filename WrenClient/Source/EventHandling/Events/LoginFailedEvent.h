#pragma once

#include <string>
#include "Event.h"

class LoginFailedEvent : public Event
{
public:
	LoginFailedEvent(const std::string& error)
		: Event(EventType::LoginFailed),
		  error{ error }
	{
	}
	const std::string& error;
};