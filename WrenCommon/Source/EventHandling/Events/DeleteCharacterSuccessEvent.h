#pragma once

#include "Event.h"

class DeleteCharacterSuccessEvent : public Event
{
public:
	DeleteCharacterSuccessEvent(std::vector<std::unique_ptr<std::string>>& characterList)
		: Event(EventType::DeleteCharacterSuccess),
		  characterList{ std::move(characterList) }
	{
	}
	const std::vector<std::unique_ptr<std::string>> characterList;
};