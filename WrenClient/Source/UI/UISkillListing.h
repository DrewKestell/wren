#pragma once

#include <Models/Skill.h>
#include "UIComponent.h"

class UISkillListing : public UIComponent
{
	int value;
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ComPtr<IDWriteTextLayout> valueTextLayout;
	ID2D1SolidColorBrush* textBrush;
	ID2D1DeviceContext1* d2dDeviceContext;
	IDWriteFactory2* writeFactory;
	IDWriteTextFormat* textFormat;
public:
	UISkillListing(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		WrenCommon::Skill* skill,
		ID2D1SolidColorBrush* textBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* textFormat);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void SetValue(const int value);
};