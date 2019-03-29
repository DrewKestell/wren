#pragma once

#include "Events/Event.h"
#include "EventHandler.h"

class Observer
{
protected:
	EventHandler& eventHandler;
	Observer(EventHandler& eventHandler)
		: eventHandler{ eventHandler }
	{
		eventHandler.Subscribe(*this);
	}
public:	
	virtual void HandleEvent(const Event& event) = 0;
};