#pragma once

#include "Events/Event.h"
#include "EventHandler.h"

extern EventHandler g_eventHandler;

class Observer
{
protected:
	Observer()
	{
		g_eventHandler.Subscribe(*this);
	}
public:	
	virtual bool HandleEvent(const Event* event) = 0;
	~Observer()
	{
		g_eventHandler.Unsubscribe(*this);
	}
};