#pragma once

#include "Events/Event.h"

class Observer;

class EventHandler
{	
public:
	void Subscribe(Observer& observer) { observers->push_back(&observer); }
	void Unsubscribe(Observer& observer) { observers->remove(&observer); }
	void QueueEvent(const Event* const event) { eventQueue->push(event); }
	std::queue<const Event*>* eventQueue = new std::queue<const Event*>;
	std::list<Observer*>* observers = new std::list<Observer*>;
};