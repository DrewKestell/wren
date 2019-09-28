#pragma once

#include <EventHandling/Events/Event.h>
#include <vector>

class NpcDeathEvent : public Event
{
public:
	NpcDeathEvent(const int gameObjectId, std::vector<int> itemIds)
		: Event(EventType::NpcDeath),
		  gameObjectId{ gameObjectId },
		  itemIds{ itemIds }
	{
	}
	const int gameObjectId;
	const std::vector<int> itemIds;
};