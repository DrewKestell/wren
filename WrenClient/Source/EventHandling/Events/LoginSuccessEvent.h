#pragma once

#include "Event.h"

class LoginSuccessEvent : public Event
{
public:
	LoginSuccessEvent(const std::string* token, const std::vector<std::string*>* characterList)
		: Event(EventType::LoginSuccess),
		  token{ token },
		  characterList{ characterList }
	{
	}
	~LoginSuccessEvent()
	{
		delete token;
		for (auto i = 0; i < characterList->size(); i++)
			delete characterList->at(i);
		delete characterList;
	}
	const std::string* token;
	const std::vector<std::string*>* characterList;
};