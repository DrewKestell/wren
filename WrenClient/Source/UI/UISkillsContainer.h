#pragma once

#include "UIComponent.h"
#include "UISkillListing.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"
#include <Models/Skill.h>

class UISkillsContainer : public UIComponent
{
	ID2D1SolidColorBrush* brush{ nullptr };
	IDWriteTextFormat* textFormat{ nullptr };
	std::vector<std::unique_ptr<UISkillListing>> skillListings;
public:
	UISkillsContainer(UIComponentArgs uiComponentArgs);
	void Initialize(
		ID2D1SolidColorBrush* brush,
		IDWriteTextFormat* textFormat,
		const std::vector<std::unique_ptr<WrenCommon::Skill>>& skills);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void Initialize();
};