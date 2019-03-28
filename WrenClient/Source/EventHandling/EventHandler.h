#pragma once

#include <forward_list>
#include "Events/Event.h"

class Observer;

class EventHandler
{
	Layer activeLayer;
	std::forward_list<Observer*> observers;
public:
	void Subscribe(Observer* observer) { observers.push_front(observer); }
	void PublishEvent(const Event& event, const Layer layer);
	void SetActiveLayer(const Layer newLayer) { activeLayer = newLayer; }
};