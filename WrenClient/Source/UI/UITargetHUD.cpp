#include "stdafx.h"
#include "UITargetHUD.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SetTargetEvent.h"
#include "EventHandling/Events/MouseEvent.h"

using namespace DX;

UITargetHUD::UITargetHUD(UIComponentArgs uiComponentArgs)
	: UIComponent(uiComponentArgs)
{
}

void UITargetHUD::Initialize(
	IDWriteTextFormat* nameTextFormat,
	ID2D1SolidColorBrush* healthBrush,
	ID2D1SolidColorBrush* manaBrush,
	ID2D1SolidColorBrush* staminaBrush,
	ID2D1SolidColorBrush* statBackgroundBrush,
	ID2D1SolidColorBrush* statBorderBrush,
	ID2D1SolidColorBrush* nameBrush,
	ID2D1SolidColorBrush* whiteBrush
)
{
	this->nameTextFormat = nameTextFormat;
	this->healthBrush = healthBrush;
	this->manaBrush = manaBrush;
	this->staminaBrush = staminaBrush;
	this->statBackgroundBrush = statBackgroundBrush;
	this->statBorderBrush = statBorderBrush;
	this->nameBrush = nameBrush;
	this->whiteBrush = whiteBrush;

	const auto position = GetWorldPosition();
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x, position.y + 10.0f, position.x + 160.0f, position.y + 80.0f), statsContainerGeometry.ReleaseAndGetAddressOf());
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 34.0f, position.x + 152.0f, position.y + 44.0f), maxHealthGeometry.ReleaseAndGetAddressOf());
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 48.0f, position.x + 152.0f, position.y + 58.0f), maxManaGeometry.ReleaseAndGetAddressOf());
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 62.0f, position.x + 152.0f, position.y + 72.0f), maxStaminaGeometry.ReleaseAndGetAddressOf());
}

void UITargetHUD::Draw()
{
	if (!isVisible) return;

	const auto position = GetWorldPosition();

	deviceResources->GetD2DDeviceContext()->FillGeometry(statsContainerGeometry.Get(), statBackgroundBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(statsContainerGeometry.Get(), statBorderBrush, 2.0f);

	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x + 6.0f, position.y + 16.0f), nameTextLayout.Get(), nameBrush);

	const auto healthPercent = Utility::Max(0.0f, (float)statsComponent->health / (float)statsComponent->maxHealth);
	auto statPosX = 144.0f * healthPercent;
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 34.0f, position.x + 8.0f + statPosX, position.y + 44.0f), healthGeometry.ReleaseAndGetAddressOf());

	deviceResources->GetD2DDeviceContext()->FillGeometry(maxHealthGeometry.Get(), whiteBrush);
	deviceResources->GetD2DDeviceContext()->FillGeometry(healthGeometry.Get(), healthBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(maxHealthGeometry.Get(), nameBrush, 2.0f);

	const auto manaPercent = Utility::Max(0.0f, (float)statsComponent->mana / (float)statsComponent->maxMana);
	statPosX = 144.0f * manaPercent;
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 48.0f, position.x + 8.0f + statPosX, position.y + 58.0f), manaGeometry.ReleaseAndGetAddressOf());

	deviceResources->GetD2DDeviceContext()->FillGeometry(maxManaGeometry.Get(), whiteBrush);
	deviceResources->GetD2DDeviceContext()->FillGeometry(manaGeometry.Get(), manaBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(maxManaGeometry.Get(), nameBrush, 2.0f);

	const auto staminaPercent = Utility::Max(0.0f, (float)statsComponent->stamina / (float)statsComponent->maxStamina);
	statPosX = 144.0f * staminaPercent;
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 62.0f, position.x + 8.0f + statPosX, position.y + 72.0f), staminaGeometry.ReleaseAndGetAddressOf());

	deviceResources->GetD2DDeviceContext()->FillGeometry(maxStaminaGeometry.Get(), whiteBrush);
	deviceResources->GetD2DDeviceContext()->FillGeometry(staminaGeometry.Get(), staminaBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(maxStaminaGeometry.Get(), nameBrush, 2.0f);
}

const bool UITargetHUD::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				// if either the stats container is clicked, return true to stop event propagation
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y + 10.0f, position.x + 160.0f, position.y + 80.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
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
				isActive = true;
			else
			{
				isActive = false;
				isVisible = false;
			}

			break;
		}
		case EventType::SetTarget:
		{
			const auto derivedEvent = (SetTargetEvent*)event;

			const auto targetName = derivedEvent->targetName;
			ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(
				Utility::s2ws(targetName).c_str(),
				static_cast<unsigned int>(targetName.size()),
				nameTextFormat,
				200,
				100,
				nameTextLayout.ReleaseAndGetAddressOf())
			);

			statsComponent = derivedEvent->statsComponent;

			if (isActive)
				isVisible = true;

			break;
		}
		case EventType::UnsetTarget:
		{
			isVisible = false;

			break;
		}
	}

	return false;
}