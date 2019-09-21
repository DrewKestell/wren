#include "stdafx.h"
#include "UITargetHUD.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SetTargetEvent.h"

using namespace DX;

UITargetHUD::UITargetHUD(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* nameTextFormat,
	ID2D1Factory2* d2dFactory,
	ID2D1SolidColorBrush* healthBrush,
	ID2D1SolidColorBrush* manaBrush,
	ID2D1SolidColorBrush* staminaBrush,
	ID2D1SolidColorBrush* statBackgroundBrush,
	ID2D1SolidColorBrush* statBorderBrush,
	ID2D1SolidColorBrush* nameBrush,
	ID2D1SolidColorBrush* whiteBrush)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	d2dDeviceContext{ d2dDeviceContext },
	d2dFactory{ d2dFactory },
	writeFactory{ writeFactory },
	nameTextFormat{ nameTextFormat },
	healthBrush{ healthBrush },
	manaBrush{ manaBrush },
	staminaBrush{ staminaBrush },
	statBackgroundBrush{ statBackgroundBrush },
	statBorderBrush{ statBorderBrush },
	nameBrush{ nameBrush },
	whiteBrush{ whiteBrush }
{
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y + 10.0f, position.x + 160.0f, position.y + 80.0f), statsContainerGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 34.0f, position.x + 152.0f, position.y + 44.0f), maxHealthGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 48.0f, position.x + 152.0f, position.y + 58.0f), maxManaGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 62.0f, position.x + 152.0f, position.y + 72.0f), maxStaminaGeometry.ReleaseAndGetAddressOf());
}

void UITargetHUD::Draw()
{
	if (!isVisible) return;

	const auto position = GetWorldPosition();

	d2dDeviceContext->FillGeometry(statsContainerGeometry.Get(), statBackgroundBrush);
	d2dDeviceContext->DrawGeometry(statsContainerGeometry.Get(), statBorderBrush, 2.0f);

	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 6.0f, position.y + 16.0f), nameTextLayout.Get(), nameBrush);

	const auto healthPercent = Utility::Max(0.0f, (float)statsComponent->health / (float)statsComponent->maxHealth);
	auto statPosX = 144.0f * healthPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 34.0f, position.x + 8.0f + statPosX, position.y + 44.0f), healthGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxHealthGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(healthGeometry.Get(), healthBrush);
	d2dDeviceContext->DrawGeometry(maxHealthGeometry.Get(), nameBrush, 2.0f);

	const auto manaPercent = Utility::Max(0.0f, (float)statsComponent->mana / (float)statsComponent->maxMana);
	statPosX = 144.0f * manaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 48.0f, position.x + 8.0f + statPosX, position.y + 58.0f), manaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxManaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(manaGeometry.Get(), manaBrush);
	d2dDeviceContext->DrawGeometry(maxManaGeometry.Get(), nameBrush, 2.0f);

	const auto staminaPercent = Utility::Max(0.0f, (float)statsComponent->stamina / (float)statsComponent->maxStamina);
	statPosX = 144.0f * staminaPercent;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + 8.0f, position.y + 62.0f, position.x + 8.0f + statPosX, position.y + 72.0f), staminaGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(maxStaminaGeometry.Get(), whiteBrush);
	d2dDeviceContext->FillGeometry(staminaGeometry.Get(), staminaBrush);
	d2dDeviceContext->DrawGeometry(maxStaminaGeometry.Get(), nameBrush, 2.0f);
}

const bool UITargetHUD::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isActive = true;
			else
				isActive = false;

			break;
		}
		case EventType::SetTarget:
		{
			const auto derivedEvent = (SetTargetEvent*)event;

			std::wostringstream nameText;
			nameText << derivedEvent->targetName.c_str();
			ThrowIfFailed(writeFactory->CreateTextLayout(
				nameText.str().c_str(),
				(UINT32)nameText.str().size(),
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