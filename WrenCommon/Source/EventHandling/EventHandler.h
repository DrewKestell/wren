#pragma once

#include "Events/Event.h"

class Observer;

class EventHandler
{
	std::list<Observer*> observers;
	std::queue<const Event*> eventQueue;
	void PublishEvent(const Event* const event);
public:
	void Subscribe(Observer& observer) { observers.push_back(&observer); }
	void Unsubscribe(Observer& observer) { observers.remove(&observer); }
	void QueueEvent(const Event* const event) { eventQueue.push(event); }
	void PublishEvents();
};