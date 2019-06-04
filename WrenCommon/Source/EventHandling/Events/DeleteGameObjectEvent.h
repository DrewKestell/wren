#pragma once

#include "Event.h"

class DeleteGameObjectEvent : public Event
{
public:
	DeleteGameObjectEvent(const long gameObjectId)
		: Event(EventType::DeleteGameObject),
		  gameObjectId{ gameObjectId }
	{
	}
	const long gameObjectId;
};