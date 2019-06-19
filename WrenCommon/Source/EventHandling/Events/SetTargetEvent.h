#pragma once

#include <string>
#include "Event.h"
#include "Components/StatsComponent.h"

class SetTargetEvent : public Event
{
public:
	SetTargetEvent(const long gameObjectId, StatsComponent* statsComponent)
		: Event(EventType::SetTarget),
		  gameObjectId{ gameObjectId },
		  statsComponent{ statsComponent }
	{
	}
	~SetTargetEvent() { }
	const long gameObjectId;
	StatsComponent* statsComponent;
};