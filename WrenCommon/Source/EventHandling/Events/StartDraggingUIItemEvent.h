#pragma once

class StartDraggingUIItemEvent : public Event
{
public:
	StartDraggingUIItemEvent(const char slot)
		: Event(EventType::StartDraggingUIItem),
		slot{ slot }
	{
	}
	const char slot;
};