#pragma once

#include <string>
#include "Event.h"
#include "Components/StatsComponent.h"

class SetTargetEvent : public Event
{
public:
	SetTargetEvent(const unsigned int gameObjectId, std::string* targetName, StatsComponent* statsComponent)
		: Event(EventType::SetTarget),
		  gameObjectId{ gameObjectId },
		  targetName{ targetName },
		  statsComponent{ statsComponent }
	{
	}
	~SetTargetEvent() { }
	const unsigned int gameObjectId;
	std::string* targetName;
	StatsComponent* statsComponent;
};