#include "Publisher.h"

void Publisher::PublishEvent(const Event& event)
{
	eventHandler->PublishEvent(event);
}

Publisher::~Publisher() {}