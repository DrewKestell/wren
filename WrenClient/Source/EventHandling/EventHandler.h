#pragma once

#include <forward_list>
#include "Events/Event.h"

class Observer;

class EventHandler
{
	std::forward_list<Observer*> observers;
public:
	void Subscribe(Observer& observer) { observers.push_front(&observer); }
	void Unsubscribe(Observer& observer) { observers.remove(&observer); }
	void PublishEvent(const Event& event);
};