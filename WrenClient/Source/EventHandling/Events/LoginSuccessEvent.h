#pragma once

#include <string>
#include "Event.h"

class LoginSuccessEvent : public Event
{
public:
	LoginSuccessEvent(const std::string& token, const std::vector<std::string>* characterList)
		: Event(EventType::LoginSuccess),
		  token{ token },
		  characterList{ characterList }
	{
	}
	const std::string& token;
	const std::vector<std::string>* characterList;
};