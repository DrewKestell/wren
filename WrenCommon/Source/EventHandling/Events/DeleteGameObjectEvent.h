#pragma once

#include "Event.h"

class DeleteGameObjectEvent : public Event
{
public:
	DeleteGameObjectEvent(const long gameObjectId)
		: Event(EventType::DeleteGameObjectEvent),
		  gameObjectId{ gameObjectId }
	{
	}
	long gameObjectId{ 0 };
};