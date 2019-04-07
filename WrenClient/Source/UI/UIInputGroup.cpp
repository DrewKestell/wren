#include "stdafx.h"
#include "UIInputGroup.h"
#include "UIInput.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../EventHandling/Events/SystemKeyDownEvent.h"
#include "../Layer.h"

bool UIInputGroup::HandleEvent(const Event* event)
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
		case EventType::SystemKeyDownEvent:
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
