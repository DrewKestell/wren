#pragma once

#include <EventHandling/Events/MouseEvent.h>
#include "../UI/UIItem.h"

class UIItemDroppedEvent : public MouseEvent
{
public:
	UIItemDroppedEvent(const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::UIItemDropped, mousePosX, mousePosY)
	{
	}
};