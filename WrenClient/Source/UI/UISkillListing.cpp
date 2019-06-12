#include "stdafx.h"
#include "UISkillListing.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UISkillListing::UISkillListing(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	Skill& skill,
	ID2D1SolidColorBrush* textBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* textFormat)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  textBrush{ textBrush },
	  writeFactory{ writeFactory },
	  textFormat{ textFormat },
	  d2dDeviceContext{ d2dDeviceContext }
{
	std::wostringstream nameOutText;
	nameOutText << skill.name.c_str();
	ThrowIfFailed(writeFactory->CreateTextLayout(nameOutText.str().c_str(), (UINT32)nameOutText.str().size(), textFormat, 140.0f, 20.0f, nameTextLayout.ReleaseAndGetAddressOf()));

	SetValue(skill.value);
}

void UISkillListing::Draw()
{
	if (!isVisible) return;

	// Draw Name Text
	const auto position = GetWorldPosition();
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), nameTextLayout.Get(), textBrush);

	// Draw Value Text
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 160.0f, position.y), valueTextLayout.Get(), textBrush);
}

void UISkillListing::SetValue(const int value)
{
	std::wostringstream valueOutText;
	valueOutText << std::to_string(value).c_str();
	ThrowIfFailed(writeFactory->CreateTextLayout(valueOutText.str().c_str(), (UINT32)valueOutText.str().size(), textFormat, 20.0f, 30.0f, valueTextLayout.ReleaseAndGetAddressOf()));
}

const bool UISkillListing::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}