#pragma once

#include <EventHandling/Events/MouseEvent.h>
#include "../UI/UIItem.h"

class UIItemDroppedEvent : public MouseEvent
{
public:
	UIItemDroppedEvent(std::unique_ptr<UIItem>& uiItem, const float mousePosX, const float mousePosY)
		: MouseEvent(EventType::UIItemDropped, mousePosX, mousePosY),
		  uiItem{ std::move(uiItem) }
	{
	}
	std::unique_ptr<UIItem> uiItem;
};