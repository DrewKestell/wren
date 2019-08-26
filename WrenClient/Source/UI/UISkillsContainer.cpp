#include "stdafx.h"
#include "UISkillsContainer.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Events/SkillIncreaseEvent.h"

using namespace DX;

UISkillsContainer::UISkillsContainer(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	IDWriteFactory2* writeFactory,
	ID2D1SolidColorBrush* brush,
	IDWriteTextFormat* textFormat)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  d3dDevice{ d3dDevice },
	  d3dDeviceContext{ d3dDeviceContext },
	  writeFactory{ writeFactory },
	  brush{ brush },
	  textFormat{ textFormat }
{
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

void UISkillsContainer::Initialize(std::vector<WrenCommon::Skill*>* skills)
{
	delete[] skillListings;

	skillListings = new std::unique_ptr<UISkillListing>[skills->size()];
	for (auto i = 0; i < skills->size(); i++)
	{
		const auto posX = 2.0f;
		const auto posY = (14.0f * i) + 22.0f;
		skillListings[i] = std::make_unique<UISkillListing>(uiComponents, XMFLOAT2{ posX, posY }, InGame, 3, *skills->at(i), brush, d2dDeviceContext, writeFactory, textFormat);
		this->AddChildComponent(*skillListings[i].get());
	}
}