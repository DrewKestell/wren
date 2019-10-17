#pragma once

#include <Models/Skill.h>
#include "UIComponent.h"

class UISkillListing : public UIComponent
{
	ID2D1SolidColorBrush* textBrush{ nullptr };
	IDWriteTextFormat* textFormat{ nullptr };
	ComPtr<IDWriteTextLayout> nameTextLayout;
	ComPtr<IDWriteTextLayout> valueTextLayout;
	const std::string name;
	int value;

	void CreateValueTextLayout();

public:
	UISkillListing(UIComponentArgs uiComponentArgs, WrenCommon::Skill* skill);
	void Initialize(ID2D1SolidColorBrush* textBrush, IDWriteTextFormat* textFormat);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void SetValue(const int value);
};