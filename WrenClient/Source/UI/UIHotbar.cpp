#include "stdafx.h"
#include "UIHotbar.h"
#include "Events/UIAbilityDroppedEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"

UIHotbar::UIHotbar(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	EventHandler& eventHandler,
	ID2D1SolidColorBrush* brush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	const float clientHeight)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  eventHandler{ eventHandler },
	  brush{ brush },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  clientHeight{ clientHeight }
{
	const auto width = 40.0f;
	for (auto i = 0; i < 10; i++)
	{
		d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + (i * width), position.y, position.x + (i * width) + width, position.y + width), geometry[i].ReleaseAndGetAddressOf());
	}
}

void UIHotbar::Draw()
{
	if (!isVisible) return;

	for (auto i = 0; i < 10; i++)
	{
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);
	}
}

const bool UIHotbar::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
		case EventType::UIAbilityDropped:
		{
			const auto derivedEvent = (UIAbilityDroppedEvent*)event;

			const auto index = Utility::GetHotbarIndex(clientHeight, derivedEvent->mousePosX, derivedEvent->mousePosY);

			if (index >= 0)
			{
				const auto xOffset = index * 40.0f;

				uiAbilities[index] = derivedEvent->uiAbility;
				uiAbilities[index]->SetLocalPosition(XMFLOAT2{ xOffset + 2.0f, 2.0f });
				uiAbilities[index]->SetParent(*this);

				// it's important that UIAbilities receive certain events (mouse clicks for example) before other
				// UI elements, so we reorder here to make sure the UIComponents are in the right order.
				std::unique_ptr<Event> e = std::make_unique<Event>(EventType::ReorderUIComponents);
				eventHandler.QueueEvent(e);
			}
			else
			{
				delete derivedEvent->uiAbility;
			}
			if (draggingIndex >= 0)
			{
				uiAbilities[draggingIndex] = nullptr;
				draggingIndex = -1;
			}
			
			break;
		}
		case EventType::StartDraggingUIAbility:
		{
			const auto derivedEvent = (StartDraggingUIAbilityEvent*)event;
			const auto hotbarIndex = derivedEvent->hotbarIndex;

			draggingIndex = hotbarIndex;
		}
	}

	return false;
}

void UIHotbar::DrawSprites()
{
	for (auto i = 0; i < 10; i++)
	{
		if (uiAbilities[i])
			uiAbilities[i]->DrawSprite();
	}
}

const std::string UIHotbar::GetUIAbilityDragBehavior() const
{
	return "MOVE";
}