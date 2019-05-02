#pragma once

#include "Event.h"

class PlayerCorrectionEvent : public Event
{
public:
	PlayerCorrectionEvent()
		: Event(EventType::PlayerCorrection)
	{
	}
};