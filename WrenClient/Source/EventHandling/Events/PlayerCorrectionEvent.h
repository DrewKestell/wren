#pragma once

#include "Event.h"

class PlayerCorrectionEvent : public Event
{
public:
	PlayerCorrectionEvent(const std::string* token, const std::vector<std::string*>* characterList)
		: Event(EventType::PlayerCorrection),
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