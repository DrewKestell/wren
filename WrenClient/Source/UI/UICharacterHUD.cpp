#include "stdafx.h"
#include "UICharacterHUD.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/MouseEvent.h"

using namespace DX;

UICharacterHUD::UICharacterHUD(
	UIComponentArgs uiComponentArgs,
	StatsComponent& statsComponent,
	const char* nameText)
	: UIComponent(uiComponentArgs),
	  statsComponent{ statsComponent },
	  nameText{ nameText }
{
	CreateGeometry();
}

void UICharacterHUD::Initialize(
	IDWriteTextFormat* textFormat,
	ID2D1SolidColorBrush* healthBrush,
	ID2D1SolidColorBrush* manaBrush,
	ID2D1SolidColorBrush* staminaBrush,
	ID2D1SolidColorBrush* statBackgroundBrush,
	ID2D1SolidColorBrush* statBorderBrush,
	ID2D1SolidColorBrush* nameBrush,
	ID2D1SolidColorBrush* whiteBrush)
{
	this->healthBrush = healthBrush;
	this->manaBrush = manaBrush;
	this->staminaBrush = staminaBrush;
	this->statBackgroundBrush = statBackgroundBrush;
	this->statBorderBrush = statBorderBrush;
	this->nameBrush = nameBrush;
	this->whiteBrush = whiteBrush;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(nameText).c_str(),
			static_cast<unsigned int>(nameText.size()),
			textFormat,
			200,
			100,
			nameTextLayout.ReleaseAndGetAddressOf()
		)
	);
}

void UICharacterHUD::Draw()
{
	if (!isVisible) return;

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	const auto d2dFactory = deviceResources->GetD2DFactory();
	const auto position = GetWorldPosition();

	d2dDeviceContext->FillGeometry(characterPortraitGeometry.Get(), statBackgroundBrush);
	d2dDeviceContext->DrawGeometry(characterPortraitGeometry.Get(), statBorderBrush, 2.0f);

	d2dDeviceContext->FillGeometry(statsContainerGeometry.Get(), statBackgroundBrush);
	d2dDeviceContext->DrawGeometry(statsContainerGeometry.Get(), statBorderBrush, 2.0f);

	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 86.0f, position.y + 16.0f), nameTextLayout.Get(), nameBrush);

	const auto healthPercent = Utility::Max<float>(0.0f, (float)statsComponent.health / (float)statsComponent.maxHealth);
	auto statPosX = FULL_STAT_WIDTH * healthPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 34.0f, position.x + 88.0F + statPosX, position.y + 44.0f), healthGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxHealthGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(healthGeometry.Get(), healthBrush);
	d2dDeviceContext->DrawGeometry(maxHealthGeometry.Get(), nameBrush, 2.0f);

	const auto manaPercent = Utility::Max<float>(0.0f, (float)statsComponent.mana / (float)statsComponent.maxMana);
	statPosX = FULL_STAT_WIDTH * manaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 48.0f, position.x + 88.0F + + statPosX, position.y + 58.0f), manaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxManaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(manaGeometry.Get(), manaBrush);
	d2dDeviceContext->DrawGeometry(maxManaGeometry.Get(), nameBrush, 2.0f);

	const auto staminaPercent = Utility::Max<float>(0.0f, (float)statsComponent.stamina / (float)statsComponent.maxStamina);
	statPosX = FULL_STAT_WIDTH * staminaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 62.0f, position.x + 88.0F + + statPosX, position.y + 72.0f), staminaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxStaminaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(staminaGeometry.Get(), staminaBrush);
	d2dDeviceContext->DrawGeometry(maxStaminaGeometry.Get(), nameBrush, 2.0f);
}

const bool UICharacterHUD::HandleEvent(const Event* const event)
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
				// if either the character portrait or the stats container are clicked, return true to stop event propagation
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y, position.x + 80.0f, position.y + 80.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY)
					|| Utility::DetectClick(position.x + 80.0f, position.y + 10.0f, position.x + 240.0f, position.y + 80.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
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
		case EventType::WindowResize:
		{
			CreateGeometry();

			break;
		}
	}

	return false;
}

void UICharacterHUD::CreateGeometry()
{
	const auto d2dFactory = deviceResources->GetD2DFactory();
	const auto position = GetWorldPosition();

	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + 80.0f, position.y + 80.0f), characterPortraitGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 80.0f, position.y + 10.0f, position.x + 240.0f, position.y + 80.0f), statsContainerGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 34.0f, position.x + 232.0f, position.y + 44.0f), maxHealthGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 48.0f, position.x + 232.0f, position.y + 58.0f), maxManaGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 62.0f, position.x + 232.0f, position.y + 72.0f), maxStaminaGeometry.ReleaseAndGetAddressOf());
}