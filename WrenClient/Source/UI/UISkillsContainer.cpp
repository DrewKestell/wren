#include "stdafx.h"
#include "UISkillsContainer.h"
#include "../Events/SkillIncreaseEvent.h"

using namespace DX;

UISkillsContainer::UISkillsContainer(UIComponentArgs uiComponentArgs)
	: UIComponent(uiComponentArgs)
{
}

void UISkillsContainer::Initialize(
	ID2D1SolidColorBrush* brush,
	IDWriteTextFormat* textFormat,
	const std::vector<std::unique_ptr<WrenCommon::Skill>>& skills)
{
	this->brush = brush;
	this->textFormat = textFormat;

	delete[] skillListings;

	skillListings = new std::unique_ptr<UISkillListing>[skills.size()];
	for (auto i = 0; i < skills.size(); i++)
	{
		const auto posX = 2.0f;
		const auto posY = (14.0f * i) + 22.0f;
		skillListings[i] = std::make_unique<UISkillListing>(UIComponentArgs{ deviceResources, uiComponents, [posX, posY](const float, const float) { return XMFLOAT2{ posX, posY }; }, InGame, 3 }, skills.at(i).get());
		skillListings[i]->Initialize(brush, textFormat);
		this->AddChildComponent(*skillListings[i].get());
	}
}

void UISkillsContainer::Draw()
{
}

const bool UISkillsContainer::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			isVisible = false;

			break;
		}
		case EventType::SkillIncrease:
		{
			auto derivedEvent = (SkillIncreaseEvent*)event;

			skillListings[derivedEvent->skillId]->SetValue(derivedEvent->newValue);

			break;
		}
	}

	return false;
}
