#include "stdafx.h"
#include "UISkillsContainer.h"
#include "../Events/SkillIncreaseEvent.h"

using namespace DX;

UISkillsContainer::UISkillsContainer(UIComponentArgs uiComponentArgs)
	: UIComponent(uiComponentArgs)
{
}

void UISkillsContainer::Initialize(ID2D1SolidColorBrush* brush, IDWriteTextFormat* textFormat)
{
	this->brush = brush;
	this->textFormat = textFormat;

	if (skills)
		InitializeSkillListings();
}

void UISkillsContainer::Draw()
{
	// nothing to draw - UISkillListings draw themselves
}

const bool UISkillsContainer::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

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

void UISkillsContainer::SetSkills(const std::vector<std::unique_ptr<WrenCommon::Skill>>* skills)
{
	this->skills = skills;
	CreateSkillListings();
	InitializeSkillListings();
}

void UISkillsContainer::CreateSkillListings()
{
	skillListings.clear();
	ClearChildren();

	const auto posX = 2.0f;
	for (auto i = 0; i < skills->size(); i++)
	{
		const auto posY = (14.0f * i) + 22.0f;
		skillListings.push_back(std::make_unique<UISkillListing>(UIComponentArgs{ deviceResources, uiComponents, [posX, posY](const float, const float) { return XMFLOAT2{ posX, posY }; }, InGame, zIndex + 1 }, skills->at(i).get()));
		this->AddChildComponent(*skillListings[i].get());
	}

	InitializeSkillListings();
}

void UISkillsContainer::InitializeSkillListings()
{
	for (auto i = 0; i < skills->size(); i++)
	{
		skillListings.at(i)->Initialize(brush, textFormat);
		skillListings.at(i)->isVisible = isVisible;
	}
}
