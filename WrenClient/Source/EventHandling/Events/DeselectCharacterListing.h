#pragma once

#include "Event.h"

class DeselectCharacterListing : public Event
{
public:
	DeselectCharacterListing()
		: Event(EventType::SelectCharacterListing)
	{
	}
};