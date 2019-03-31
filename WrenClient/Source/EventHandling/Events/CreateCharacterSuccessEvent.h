#pragma once

#include <string>
#include <vector>
#include "Event.h"

class CreateCharacterSuccessEvent : public Event
{
public:
	CreateCharacterSuccessEvent(const std::vector<std::string>* characterList)
		: Event(EventType::CreateCharacterSuccess),
		  characterList{ characterList }
	{
	}
	const std::vector<std::string>* characterList;
};