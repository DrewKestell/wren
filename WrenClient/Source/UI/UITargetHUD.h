#pragma once

#include "../Components/StatsComponent.h"
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
	ID2D1DeviceContext1* d2dDeviceContext;
	IDWriteTextFormat* buttonTextFormat;
	ID2D1Factory2* d2dFactory;
	IDWriteFactory2* writeFactory;
	StatsComponent* statsComponent;
public:
	UITargetHUD(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* buttonTextFormat,
		ID2D1Factory2* d2dFactory,
		ID2D1SolidColorBrush* healthBrush,
		ID2D1SolidColorBrush* manaBrush,
		ID2D1SolidColorBrush* staminaBrush,
		ID2D1SolidColorBrush* statBackgroundBrush,
		ID2D1SolidColorBrush* statBorderBrush,
		ID2D1SolidColorBrush* nameBrush,
		ID2D1SolidColorBrush* whiteBrush);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};