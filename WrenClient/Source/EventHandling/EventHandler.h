#pragma once

#include <forward_list>
#include <queue>
#include "Events/Event.h"

class Observer;

class EventHandler
{
	std::forward_list<Observer*> observers;
	std::queue<const Event*> eventQueue;
	void PublishEvent(const Event* event);
public:
	void Subscribe(Observer& observer) { observers.push_front(&observer); }
	void Unsubscribe(Observer& observer) { observers.remove(&observer); }
	void QueueEvent(const Event* event) { eventQueue.push(event); }
	void PublishEvents();
};