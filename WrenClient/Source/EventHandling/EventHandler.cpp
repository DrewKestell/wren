#include "stdafx.h"
#include "EventHandler.h"
#include "Observer.h"

void EventHandler::PublishEvent(const Event* event)
{
	for (auto it = observers.begin(); it != observers.end(); it++)
	{
		const auto stopPropagation = (*it)->HandleEvent(event);
		if (stopPropagation)
			break;
	}
}

void EventHandler::PublishEvents()
{
	while (!eventQueue.empty())
	{
		const auto event = eventQueue.front();
		eventQueue.pop();
		PublishEvent(event);
	}
}
