#pragma once

#include "Event.h"

class PlayerCorrectionEvent : public Event
{
public:
	PlayerCorrectionEvent(const int updateId, const float posX, const float posY, const float posZ)
		: Event(EventType::PlayerCorrection),
		  updateId{ updateId },
		  posX{ posX }, posY{ posY }, posZ{ posZ }
	{
	}
	const int updateId;
	const float posX;
	const float posY;
	const float posZ;
};