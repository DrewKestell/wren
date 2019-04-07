#pragma once

#include "Event.h"

class CreateCharacterFailedEvent : public Event
{
public:
	CreateCharacterFailedEvent(const std::string* error)
		: Event(EventType::CreateCharacterFailed),
		  error{ error }
	{
	}
	~CreateCharacterFailedEvent() { delete error; }
	const std::string* error;
};