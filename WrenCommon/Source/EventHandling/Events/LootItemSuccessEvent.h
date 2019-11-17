#pragma once

#include <EventHandling/Events/Event.h>

class LootItemSuccessEvent : public Event
{
public:
	LootItemSuccessEvent(const int looteeId, const int slot, const int destinationSlot, const int itemId, const int looterId)
		: Event(EventType::LootItemSuccess),
		  looteeId{ looteeId },
		  slot{ slot },
		  destinationSlot{ destinationSlot },
		  itemId{ itemId },
		  looterId{ looterId }
	{
	}
	const int looteeId;
	const int slot;
	const int destinationSlot;
	const int itemId;
	const int looterId;
};