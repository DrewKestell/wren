#include "stdafx.h"
#include "UIInputGroup.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"

UIInputGroup::UIInputGroup(const Layer uiLayer, EventHandler& eventHandler)
	: uiLayer{ uiLayer },
	  eventHandler{ eventHandler }
{
	eventHandler.Subscribe(*this);
}

const bool UIInputGroup::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer)
				active = true;
			else
				active = false;

			break;
		}
		case EventType::SystemKeyDown:
		{
			const auto derivedEvent = (SystemKeyDownEvent*)event;

			auto activeInputExists = false;
			if (active && derivedEvent->keyCode == VK_TAB)
			{
				for (auto it = inputs.begin(); it != inputs.end(); it++)
				{
					if ((it + 1) != inputs.end() && (*it)->IsActive())
					{
						(*it)->SetActive(false);
						(*it)->CreateTextLayout();
						(*(it + 1))->SetActive(true);
						(*(it + 1))->CreateTextLayout();
						activeInputExists = true;
						break;
					}
					if ((it + 1) == inputs.end())
					{
						(*it)->SetActive(false);
						(*it)->CreateTextLayout();
					}
				}
				if (!activeInputExists)
				{
					(*inputs.begin())->SetActive(true);
					(*inputs.begin())->CreateTextLayout();
				}

				return true;
			}

			break;
		}
	}

	return false;
}

UIInputGroup::~UIInputGroup()
{
	eventHandler.Unsubscribe(*this);
}