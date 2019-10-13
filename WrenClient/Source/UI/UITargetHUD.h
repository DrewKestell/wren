#pragma once

#include <Components/StatsComponent.h>
#include "UIComponent.h"

class UITargetHUD : public UIComponent
{
	bool isActive{ false };
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
	IDWriteTextFormat* nameTextFormat;
	StatsComponent* statsComponent;
public:
	UITargetHUD(
		UIComponentArgs uiComponentArgs,
		IDWriteTextFormat* nameTextFormat,
		ID2D1SolidColorBrush* healthBrush,
		ID2D1SolidColorBrush* manaBrush,
		ID2D1SolidColorBrush* staminaBrush,
		ID2D1SolidColorBrush* statBackgroundBrush,
		ID2D1SolidColorBrush* statBorderBrush,
		ID2D1SolidColorBrush* nameBrush,
		ID2D1SolidColorBrush* whiteBrush);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};