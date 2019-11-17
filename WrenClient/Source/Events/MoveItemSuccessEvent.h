#pragma once

#include <EventHandling/Events/Event.h>

class MoveItemSuccessEvent : public Event
{
public:
	MoveItemSuccessEvent(const int draggingSlot, const int slot)
		: Event(EventType::MoveItemSuccess),
		  draggingSlot{ draggingSlot },
		  slot{ slot }
	{
	}
	const int draggingSlot;
	const int slot;
};