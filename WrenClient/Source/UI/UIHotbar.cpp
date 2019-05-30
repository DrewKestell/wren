#include "stdafx.h"
#include "UIHotbar.h"
#include "Layer.h"
#include "Events/UIAbilityDroppedEvent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UIHotbar::UIHotbar(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	ID2D1SolidColorBrush* brush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	brush{ brush },
	d2dDeviceContext{ d2dDeviceContext },
	d2dFactory{ d2dFactory }
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

			const auto index = GetIndex(derivedEvent->mousePosX, derivedEvent->mousePosY);
			const auto xOffset = index * 40.0f;

			uiAbilities[index] = derivedEvent->uiAbility;
			uiAbilities[index]->SetLocalPosition(XMFLOAT3{ xOffset + 2.0f, 2.0f, 0.0f });
			uiAbilities[index]->SetParent(*this);

			break;
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

const int UIHotbar::GetIndex(const float posX, const float posY) const
{
	const auto worldPos = GetWorldPosition();

	if (posX < worldPos.x || posX >= worldPos.x + 400.0f || posY < worldPos.y || posY > worldPos.y + 40.0f)
		return -1;

	return (posX - 5) / 40;
}

const std::string UIHotbar::GetUIAbilityDragBehavior() const
{
	return "MOVE";
}