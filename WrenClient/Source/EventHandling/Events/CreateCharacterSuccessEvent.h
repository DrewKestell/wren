#pragma once

#include "Event.h"

class CreateCharacterSuccessEvent : public Event
{
public:
	CreateCharacterSuccessEvent(const std::vector<std::string*>* characterList)
		: Event(EventType::CreateCharacterSuccess),
		  characterList{ characterList }
	{
	}
	~CreateCharacterSuccessEvent()
	{
		for (auto i = 0; i < characterList->size(); i++)
			delete characterList->at(i);
		delete characterList;
	}
	const std::vector<std::string*>* characterList;
};