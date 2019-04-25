#pragma once

#include "Event.h"

class LoginSuccessEvent : public Event
{
public:
	LoginSuccessEvent(const std::vector<std::string*>* characterList)
		: Event(EventType::LoginSuccess),
		  characterList{ characterList }
	{
	}
	~LoginSuccessEvent()
	{
		for (auto i = 0; i < characterList->size(); i++)
			delete characterList->at(i);
		delete characterList;
	}
	const std::vector<std::string*>* characterList;
};