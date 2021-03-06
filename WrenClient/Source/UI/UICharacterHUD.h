#pragma once

#include <Components/StatsComponent.h>
#include "UIComponent.h"

const auto FULL_STAT_WIDTH = 144.0f;

class UICharacterHUD : public UIComponent
{
	StatsComponent& statsComponent;
	std::string nameText;
	ID2D1SolidColorBrush* healthBrush{ nullptr };
	ID2D1SolidColorBrush* manaBrush{ nullptr };
	ID2D1SolidColorBrush* staminaBrush{ nullptr };
	ID2D1SolidColorBrush* statBackgroundBrush{ nullptr };
	ID2D1SolidColorBrush* statBorderBrush{ nullptr };
	ID2D1SolidColorBrush* nameBrush{ nullptr };
	ID2D1SolidColorBrush* whiteBrush{ nullptr };
	ComPtr<ID2D1RectangleGeometry> characterPortraitGeometry;
	ComPtr<ID2D1RectangleGeometry> statsContainerGeometry;
	ComPtr<ID2D1RectangleGeometry> healthGeometry;
	ComPtr<ID2D1RectangleGeometry> maxHealthGeometry;
	ComPtr<ID2D1RectangleGeometry> manaGeometry;
	ComPtr<ID2D1RectangleGeometry> maxManaGeometry;
	ComPtr<ID2D1RectangleGeometry> staminaGeometry;
	ComPtr<ID2D1RectangleGeometry> maxStaminaGeometry;
	ComPtr<IDWriteTextLayout> nameTextLayout;
	
public:
	UICharacterHUD(
		UIComponentArgs uiComponentArgs,
		StatsComponent& statsComponent,
		const char* nameText);
	void Initialize(
		IDWriteTextFormat* textFormat,
		ID2D1SolidColorBrush* healthBrush,
		ID2D1SolidColorBrush* manaBrush,
		ID2D1SolidColorBrush* staminaBrush,
		ID2D1SolidColorBrush* statBackgroundBrush,
		ID2D1SolidColorBrush* statBorderBrush,
		ID2D1SolidColorBrush* nameBrush,
		ID2D1SolidColorBrush* whiteBrush);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void CreateGeometry();
};