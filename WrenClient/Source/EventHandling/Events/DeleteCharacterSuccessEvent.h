#pragma once

#include <string>
#include <vector>
#include "Event.h"

class DeleteCharacterSuccessEvent : public Event
{
public:
	DeleteCharacterSuccessEvent(const std::vector<std::string*>* characterList)
		: Event(EventType::DeleteCharacterSuccess),
		characterList{ characterList }
	{
	}
	~DeleteCharacterSuccessEvent()
	{
		for (auto i = 0; i < characterList->size(); i++)
			delete characterList->at(i);
		delete characterList;
	}
	const std::vector<std::string*>* characterList;
};