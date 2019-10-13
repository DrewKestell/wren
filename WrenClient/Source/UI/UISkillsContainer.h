#pragma once

#include "UIComponent.h"
#include "UISkillListing.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"
#include <Models/Skill.h>

class UISkillsContainer : public UIComponent
{
	bool initialized{ false };
	ID2D1SolidColorBrush* brush;
	IDWriteTextFormat* textFormat;
	std::unique_ptr<UISkillListing>* skillListings;
public:
	UISkillsContainer(
		UIComponentArgs uiComponentArgs,
		ID2D1SolidColorBrush* brush,
		IDWriteTextFormat* textFormat);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void Initialize(const std::vector<std::unique_ptr<WrenCommon::Skill>>& skills);
};