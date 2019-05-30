#include "stdafx.h"
#include "UICharacterHUD.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UICharacterHUD::UICharacterHUD(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* buttonTextFormat,
	ID2D1Factory2* d2dFactory,
	StatsComponent* statsComponent,
	ID2D1SolidColorBrush* healthBrush,
	ID2D1SolidColorBrush* manaBrush,
	ID2D1SolidColorBrush* staminaBrush,
	ID2D1SolidColorBrush* statBackgroundBrush,
	ID2D1SolidColorBrush* statBorderBrush,
	ID2D1SolidColorBrush* nameBrush,
	const char* inNameText)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  statsComponent{ statsComponent },
	  healthBrush{ healthBrush },
	  manaBrush{ manaBrush },
	  staminaBrush{ staminaBrush },
	  statBackgroundBrush{ statBackgroundBrush },
	  statBorderBrush{ statBorderBrush },
	  nameBrush{ nameBrush }
{
	std::wostringstream nameText;
	buttonText << inNameText;
	ThrowIfFailed(writeFactory->CreateTextLayout(
		buttonText.str().c_str(),
		(UINT32)buttonText.str().size(),
		buttonTextFormat,
		width,
		height,
		buttonTextLayout.ReleaseAndGetAddressOf())
	);
}

void UICharacterHUD::Draw()
{

}

const bool UICharacterHUD::HandleEvent(const Event* const event)
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