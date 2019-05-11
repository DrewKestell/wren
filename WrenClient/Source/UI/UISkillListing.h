#pragma once

#include <Models/Skill.h>
#include "UIComponent.h"

class UISkillListing : public UIComponent
{
	const std::string name;
	int value;
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ComPtr<IDWriteTextLayout> valueTextLayout;
	ID2D1SolidColorBrush* textBrush;
	ID2D1DeviceContext1* d2dDeviceContext;
public:
	UISkillListing(
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		const float width,
		const float height,
		Skill& skill,
		ID2D1SolidColorBrush* textBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* textFormat,
		ID2D1Factory2* d2dFactory);
	virtual void Draw();
	void SetValue(const int value) { this->value = value; }
};