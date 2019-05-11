#include "stdafx.h"
#include "UISkillListing.h"

using namespace DX;

UISkillListing::UISkillListing(
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const float width,
	const float height,
	Skill& skill,
	ID2D1SolidColorBrush* textBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* textFormat,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  textBrush{ textBrush },
	  d2dDeviceContext{ d2dDeviceContext }
{
	std::wostringstream nameOutText;
	nameOutText << skill.name.c_str();
	ThrowIfFailed(writeFactory->CreateTextLayout(nameOutText.str().c_str(), (UINT32)outText.str().size(), textFormat, width, height, textLayout.ReleaseAndGetAddressOf()));
}