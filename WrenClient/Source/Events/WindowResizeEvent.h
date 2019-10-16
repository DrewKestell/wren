#pragma once

#include <EventHandling/Events/Event.h>

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(const float width, const float height)
		: Event(EventType::WindowResize),
		  width{ width },
		  height{ height }
	{
	}
	const float width;
	const float height;
};