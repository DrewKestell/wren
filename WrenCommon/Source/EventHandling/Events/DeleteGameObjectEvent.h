#pragma once

#include "Event.h"

class DeleteGameObjectEvent : public Event
{
public:
	DeleteGameObjectEvent(const int gameObjectId)
		: Event(EventType::DeleteGameObject),
		  gameObjectId{ gameObjectId }
	{
	}
	const int gameObjectId;
};