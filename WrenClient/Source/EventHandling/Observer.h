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
	virtual bool HandleEvent(const Event& event) = 0;
	~Observer()
	{
		eventHandler.Unsubscribe(*this);
	}
};