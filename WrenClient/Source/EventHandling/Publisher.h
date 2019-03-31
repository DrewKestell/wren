#pragma once

#include <queue>
#include "EventHandler.h"
#include "Events/Event.h"

class Publisher
{
	std::queue<const Event*> eventQueue;
protected:
	EventHandler& eventHandler;
	Publisher(EventHandler& eventHandler)
		: eventHandler{ eventHandler }
	{
	}
	void QueueEvent(const Event* event) { eventQueue.push(event); }
	void PublishEvents();
public:
	virtual ~Publisher() = 0;
};