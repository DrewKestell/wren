#pragma once

#include <Models/Skill.h>
#include "UIComponent.h"

class UISkillListing : public UIComponent
{
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ComPtr<IDWriteTextLayout> valueTextLayout;
	ID2D1SolidColorBrush* textBrush;
	ID2D1DeviceContext1* d2dDeviceContext;
	IDWriteFactory2* writeFactory;
	IDWriteTextFormat* textFormat;
public:
	UISkillListing(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		Skill& skill,
		ID2D1SolidColorBrush* textBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* textFormat);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void SetValue(const int value);
};