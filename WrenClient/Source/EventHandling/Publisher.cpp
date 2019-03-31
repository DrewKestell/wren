#include "Publisher.h"

void Publisher::PublishEvents()
{
	while (!eventQueue.empty())
	{
		const auto event = eventQueue.front();
		eventQueue.pop();
		eventHandler.PublishEvent(event);
	}
}

Publisher::~Publisher() {}