#pragma once

#include "Event.h"
#include "../../Layer.h"

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