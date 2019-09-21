#pragma once

#include "Event.h"

class LoginSuccessEvent : public Event
{
public:
	LoginSuccessEvent(std::vector<std::unique_ptr<std::string>>& characterList)
		: Event(EventType::LoginSuccess),
		  characterList{ std::move(characterList) }
	{
	}
	const std::vector<std::unique_ptr<std::string>> characterList;
};