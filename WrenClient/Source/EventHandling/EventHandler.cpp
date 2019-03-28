#include "EventHandler.h"
#include "Observer.h">

void EventHandler::PublishEvent(const Event& event, const Layer layer)
{
	for (auto it = observers.begin(); it != observers.end(); it++)
		(*it)->HandleEvent(event, layer);
}