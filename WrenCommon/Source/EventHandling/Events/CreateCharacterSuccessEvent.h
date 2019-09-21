#pragma once

#include "Event.h"

class CreateCharacterSuccessEvent : public Event
{
public:
	CreateCharacterSuccessEvent(std::vector<std::unique_ptr<std::string>>& characterList)
		: Event(EventType::CreateCharacterSuccess),
		  characterList{ std::move(characterList) }
	{
	}
	const std::vector<std::unique_ptr<std::string>> characterList;
};