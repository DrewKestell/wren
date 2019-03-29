#pragma once

#include "Event.h"

class ChangeActiveLayerEvent : public Event
{
public:
	ChangeActiveLayerEvent(const Layer layer)
		: Event(EventType::ChangeActiveLayer),
		layer{ layer }
	{
	}
	const Layer layer;
};