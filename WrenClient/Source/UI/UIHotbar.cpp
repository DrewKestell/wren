#include "stdafx.h"
#include <Utility.h>
#include "UIHotbar.h"
#include "Layer.h"
#include "Events/UIAbilityDroppedEvent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"

UIHotbar::UIHotbar(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	ID2D1SolidColorBrush* brush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	const float clientHeight)
	: UIComponent(uiComponents, position, scale, uiLayer),
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
		case EventType::UIAbilityDroppedEvent:
		{
			const auto derivedEvent = (UIAbilityDroppedEvent*)event;

			const auto index = Utility::GetHotbarIndex(clientHeight, derivedEvent->mousePosX, derivedEvent->mousePosY);

			if (index >= 0)
			{
				const auto xOffset = index * 40.0f;

				uiAbilities[index] = derivedEvent->uiAbility;
				uiAbilities[index]->SetLocalPosition(XMFLOAT3{ xOffset + 2.0f, 2.0f, 0.0f });
				uiAbilities[index]->SetParent(*this);
			}
			else
			{
				delete derivedEvent->uiAbility;
			}
			
			break;
		}
		case EventType::StartDraggingUIAbility:
		{
			const auto derivedEvent = (StartDraggingUIAbilityEvent*)event;
			const auto hotbarIndex = derivedEvent->hotbarIndex;

			if (hotbarIndex >= 0)
				uiAbilities[hotbarIndex] = nullptr;
		}
	}

	return false;
}

void UIHotbar::DrawSprites()
{
	for (auto i = 0; i < 10; i++)
	{
		auto uiAbility = uiAbilities[i];
		if (uiAbility)
			uiAbility->DrawSprite();
	}
}

const std::string UIHotbar::GetUIAbilityDragBehavior() const
{
	return "MOVE";
}