#pragma once

#include <string>
#include "Event.h"

class CreateCharacterFailedEvent : public Event
{
public:
	CreateCharacterFailedEvent(const std::string& error)
		: Event(EventType::CreateCharacterFailed),
		  error{ error }
	{
	}
	const std::string& error;
};