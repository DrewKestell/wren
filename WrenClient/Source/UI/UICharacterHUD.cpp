#include "stdafx.h"
#include "UICharacterHUD.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UICharacterHUD::UICharacterHUD(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* buttonTextFormat,
	ID2D1Factory2* d2dFactory,
	StatsComponent& statsComponent,
	ID2D1SolidColorBrush* healthBrush,
	ID2D1SolidColorBrush* manaBrush,
	ID2D1SolidColorBrush* staminaBrush,
	ID2D1SolidColorBrush* statBackgroundBrush,
	ID2D1SolidColorBrush* statBorderBrush,
	ID2D1SolidColorBrush* nameBrush,
	ID2D1SolidColorBrush* whiteBrush,
	const char* inNameText)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  statsComponent{ statsComponent },
	  healthBrush{ healthBrush },
	  manaBrush{ manaBrush },
	  staminaBrush{ staminaBrush },
	  statBackgroundBrush{ statBackgroundBrush },
	  statBorderBrush{ statBorderBrush },
	  nameBrush{ nameBrush },
	  whiteBrush{ whiteBrush }
{
	std::wostringstream nameText;
	nameText << inNameText;
	ThrowIfFailed(writeFactory->CreateTextLayout(
		nameText.str().c_str(),
		(UINT32)nameText.str().size(),
		buttonTextFormat,
		200,
		100,
		nameTextLayout.ReleaseAndGetAddressOf())
	);

	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + 80.0f, position.y + 80.0f), characterPortraitGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 80.0f, position.y + 10.0f, position.x + 240.0f, position.y + 80.0f), statsContainerGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 34.0f, position.x + 232.0f, position.y + 44.0f), maxHealthGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 48.0f, position.x + 232.0f, position.y + 58.0f), maxManaGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 62.0f, position.x + 232.0f, position.y + 72.0f), maxStaminaGeometry.ReleaseAndGetAddressOf());
}

void UICharacterHUD::Draw()
{
	if (!isVisible) return;

	const auto position = GetWorldPosition();

	d2dDeviceContext->FillGeometry(characterPortraitGeometry.Get(), statBackgroundBrush);
	d2dDeviceContext->DrawGeometry(characterPortraitGeometry.Get(), statBorderBrush, 2.0f);

	d2dDeviceContext->FillGeometry(statsContainerGeometry.Get(), statBackgroundBrush);
	d2dDeviceContext->DrawGeometry(statsContainerGeometry.Get(), statBorderBrush, 2.0f);

	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 86.0f, position.y + 16.0f), nameTextLayout.Get(), nameBrush);

	const auto healthPercent = statsComponent.health / statsComponent.maxHealth;
	auto statPosX = 232.0f * healthPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 34.0f, position.x + statPosX, position.y + 44.0f), healthGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxHealthGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(healthGeometry.Get(), healthBrush);
	d2dDeviceContext->DrawGeometry(maxHealthGeometry.Get(), nameBrush, 2.0f);

	const auto manaPercent = statsComponent.mana / statsComponent.maxMana;
	statPosX = 232.0f * manaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 48.0f, position.x + statPosX, position.y + 58.0f), manaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxManaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(manaGeometry.Get(), manaBrush);
	d2dDeviceContext->DrawGeometry(maxManaGeometry.Get(), nameBrush, 2.0f);

	const auto staminaPercent = statsComponent.stamina / statsComponent.maxStamina;
	statPosX = 232.0f * staminaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 88.0f, position.y + 62.0f, position.x + statPosX, position.y + 72.0f), staminaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxStaminaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(staminaGeometry.Get(), staminaBrush);
	d2dDeviceContext->DrawGeometry(maxStaminaGeometry.Get(), nameBrush, 2.0f);
}

const bool UICharacterHUD::HandleEvent(const Event* const event)
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
	}

	return false;
}