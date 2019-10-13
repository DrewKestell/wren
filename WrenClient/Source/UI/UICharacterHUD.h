#pragma once

#include <Components/StatsComponent.h>
#include "UIComponent.h"

class UICharacterHUD : public UIComponent
{
	ComPtr<ID2D1RectangleGeometry> characterPortraitGeometry;
	ComPtr<ID2D1RectangleGeometry> statsContainerGeometry;
	ComPtr<ID2D1RectangleGeometry> healthGeometry;
	ComPtr<ID2D1RectangleGeometry> maxHealthGeometry;
	ComPtr<ID2D1RectangleGeometry> manaGeometry;
	ComPtr<ID2D1RectangleGeometry> maxManaGeometry;
	ComPtr<ID2D1RectangleGeometry> staminaGeometry;
	ComPtr<ID2D1RectangleGeometry> maxStaminaGeometry;
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ID2D1SolidColorBrush* healthBrush;
	ID2D1SolidColorBrush* manaBrush;
	ID2D1SolidColorBrush* staminaBrush;
	ID2D1SolidColorBrush* statBackgroundBrush;
	ID2D1SolidColorBrush* statBorderBrush;
	ID2D1SolidColorBrush* nameBrush;
	ID2D1SolidColorBrush* whiteBrush;
	IDWriteTextFormat* buttonTextFormat;
	StatsComponent& statsComponent;
public:
	UICharacterHUD(
		UIComponentArgs uiComponentArgs,
		IDWriteTextFormat* buttonTextFormat,
		StatsComponent& statsComponent,
		ID2D1SolidColorBrush* healthBrush,
		ID2D1SolidColorBrush* manaBrush,
		ID2D1SolidColorBrush* staminaBrush,
		ID2D1SolidColorBrush* statBackgroundBrush,
		ID2D1SolidColorBrush* statBorderBrush,
		ID2D1SolidColorBrush* nameBrush,
		ID2D1SolidColorBrush* whiteBrush,
		const char* inNameText);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};