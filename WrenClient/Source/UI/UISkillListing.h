#pragma once

#include <Models/Skill.h>
#include "UIComponent.h"

class UISkillListing : public UIComponent
{
	int value;
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ComPtr<IDWriteTextLayout> valueTextLayout;
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormat;
public:
	UISkillListing(
		UIComponentArgs uiComponentArgs,
		WrenCommon::Skill* skill,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormat);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void SetValue(const int value);
};