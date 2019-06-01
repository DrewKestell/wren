#include "stdafx.h"
#include "UIInputGroup.h"
#include "UIInput.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "Layer.h"

extern EventHandler g_eventHandler;

UIInputGroup::UIInputGroup(const Layer uiLayer)
	: uiLayer{ uiLayer }
{
	g_eventHandler.Subscribe(*this);
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
						(*(it + 1))->SetActive(true);
						activeInputExists = true;
						break;
					}
					if ((it + 1) == inputs.end())
						(*it)->SetActive(false);
				}
				if (!activeInputExists)
					(*inputs.begin())->SetActive(true);

				return true;
			}

			break;
		}
	}

	return false;
}

UIInputGroup::~UIInputGroup()
{
	g_eventHandler.Unsubscribe(*this);
}