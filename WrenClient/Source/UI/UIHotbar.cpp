#include "stdafx.h"
#include "UIHotbar.h"
#include "Events/UIAbilityDroppedEvent.h"
#include "../Events/WindowResizeEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"

UIHotbar::UIHotbar(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	const float clientHeight)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  clientHeight{ clientHeight }
{
}

void UIHotbar::Initialize(ID2D1SolidColorBrush* brush)
{
	this->brush = brush;
}

void UIHotbar::Draw()
{
	if (!isVisible) return;

	for (auto i = 0; i < 10; i++)
		deviceResources->GetD2DDeviceContext()->DrawGeometry(geometry[i].Get(), brush, 2.0f);
}

const bool UIHotbar::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				// if anywhere in the hotbar is clicked, return true to stop event propagation
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y, position.x + 400.0f, position.y + 40.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					return true;
				}
			}

			break;
		}
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
				if (index != draggingIndex)
					uiAbilities[draggingIndex] = nullptr;

				draggingIndex = -1;
			}
			
			break;
		}
		case EventType::StartDraggingUIAbility:
		{
			const auto derivedEvent = (StartDraggingUIAbilityEvent*)event;

			draggingIndex = derivedEvent->hotbarIndex;

			break;
		}
		case EventType::WindowResize:
		{
			const auto derivedEvent = (WindowResizeEvent*)event;

			clientHeight = derivedEvent->height;

			const auto position = GetWorldPosition();
			const auto width = 40.0f;

			for (auto i = 0; i < 10; i++)
				deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + (i * width), position.y, position.x + (i * width) + width, position.y + width), geometry[i].ReleaseAndGetAddressOf());

			break;
		}
	}

	return false;
}

const std::string UIHotbar::GetUIAbilityDragBehavior() const
{
	return "MOVE";
}