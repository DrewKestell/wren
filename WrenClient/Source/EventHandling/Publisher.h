#pragma once

#include "EventHandler.h"
#include "Events/Event.h"

class Publisher
{
protected:
	EventHandler* eventHandler;
	Publisher(EventHandler* eventHandler)
		: eventHandler{ eventHandler }
	{
	}
	void PublishEvent(const Event& event);
public:
	virtual ~Publisher() = 0;
};