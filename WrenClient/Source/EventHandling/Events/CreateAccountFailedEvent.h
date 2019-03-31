#pragma once

#include <string>
#include "Event.h"

class CreateAccountFailedEvent : public Event
{
public:
	CreateAccountFailedEvent(const std::string& error)
		: Event(EventType::CreateAccountFailed),
		error{ error }
	{
	}
	const std::string& error;
};