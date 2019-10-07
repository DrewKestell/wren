#pragma once

#include <EventHandling/Events/Event.h>

class LootItemSuccessEvent : public Event
{
public:
	LootItemSuccessEvent(const int gameObjectId, const int slot, const int destinationSlot, const int itemId, const int looterId)
		: Event(EventType::LootItemSuccess),
		  gameObjectId{ gameObjectId },
		  slot{ slot },
		  destinationSlot{ destinationSlot },
		  itemId{ itemId },
		  looterId{ looterId }
	{
	}
	const int gameObjectId;
	const int slot;
	const int destinationSlot;
	const int itemId;
	const int looterId;
};