#include "stdafx.h"
#include "UISkillListing.h"

using namespace DX;

UISkillListing::UISkillListing(
	UIComponentArgs uiComponentArgs,
	WrenCommon::Skill* skill,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormat)
	: UIComponent(uiComponentArgs),
	  value{ skill->value },
	  textBrush{ textBrush },
	  textFormat{ textFormat }
{
	std::wostringstream nameOutText;
	nameOutText << skill->name.c_str();
	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(nameOutText.str().c_str(), (UINT32)nameOutText.str().size(), textFormat, 140.0f, 20.0f, nameTextLayout.ReleaseAndGetAddressOf()));

	SetValue(skill->value);
}

void UISkillListing::Draw()
{
	if (!isVisible) return;

	// Draw Name Text
	const auto position = GetWorldPosition();
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x, position.y), nameTextLayout.Get(), textBrush);

	// Draw Value Text
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x + 160.0f, position.y), valueTextLayout.Get(), textBrush);
}

void UISkillListing::SetValue(const int value)
{
	std::wostringstream valueOutText;
	valueOutText << std::to_string(value).c_str();
	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(valueOutText.str().c_str(), (UINT32)valueOutText.str().size(), textFormat, 20.0f, 30.0f, valueTextLayout.ReleaseAndGetAddressOf()));
}

const bool UISkillListing::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			isVisible = false;

			break;
		}
	}

	return false;
}