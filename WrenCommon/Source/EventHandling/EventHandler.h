#pragma once

#include "Events/Event.h"

class Observer;

class EventHandler
{	
	std::queue<std::unique_ptr<const Event>> eventQueue;
	std::list<Observer*> observers;
public:
	void Subscribe(Observer& observer);
	void Unsubscribe(Observer& observer);
	void QueueEvent(std::unique_ptr<Event>& event);
	std::queue<std::unique_ptr<const Event>>& GetEventQueue();
	std::list<Observer*>& GetObservers();
};