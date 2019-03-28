#pragma once

#include "Events/Event.h"
#include "EventHandler.h"

class Observer
{
protected:
	EventHandler* eventHandler;
	Observer(EventHandler* eventHandler)
		: eventHandler{ eventHandler }
	{
		const auto foo = this;
		eventHandler->Subscribe(this);
	}
public:	
	virtual void HandleEvent(const Event& event, const Layer layer) = 0;
};