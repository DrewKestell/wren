#include "stdafx.h"
#include "UIHotbar.h"
#include "Events/UIAbilityDroppedEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"

extern float g_clientHeight;

UIHotbar::UIHotbar(UIComponentArgs uiComponentArgs, EventHandler& eventHandler)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler }
{
}

void UIHotbar::Initialize(ID2D1SolidColorBrush* brush)
{
	this->brush = brush;
}

void UIHotbar::Draw()
{
	if (!isVisible) return;

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();

	for (auto i = 0; i < 10; i++)		
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);

	for (auto i = 0; i < 10; i++)
	{
		auto uiAbility = uiAbilities[i];

		if (uiAbility)
			uiAbility->DrawHoverAndToggledBorders();
	}
}

const bool UIHotbar::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

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

			const auto index = Utility::GetHotbarIndex(g_clientHeight, derivedEvent->mousePosX, derivedEvent->mousePosY);

			if (index >= 0)
			{
				const auto xOffset = index * 40.0f;

				uiAbilities[index] = derivedEvent->uiAbility;
				uiAbilities[index]->SetLocalPosition(XMFLOAT2{ xOffset, -20.0f });
				uiAbilities[index]->SetParent(*this);
				uiAbilities[index]->CreatePositionDependentResources();

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
			const auto d2dFactory = deviceResources->GetD2DFactory();
			const auto position = GetWorldPosition();
			const auto width = 40.0f;

			for (auto i = 0; i < 10; i++)
			{
				d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + (i * width), position.y, position.x + (i * width) + width, position.y + width), geometry[i].ReleaseAndGetAddressOf());
				if (uiAbilities[i])
				{
					uiAbilities[i]->SetLocalPosition(XMFLOAT2{ i * width, -20.0f });
					uiAbilities[i]->CreatePositionDependentResources();
				}
			}

			break;
		}
	}

	return false;
}

const std::string UIHotbar::GetUIAbilityDragBehavior() const
{
	return "MOVE";
}