#include "stdafx.h"
#include "UILabel.h"
#include "UIComponent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Layer.h"

using namespace DX;

UILabel::UILabel(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const float width,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormat,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  width{ width },
	  textBrush{ textBrush },
	  textFormat{ textFormat },	
	  writeFactory{ writeFactory },
	  d2dDeviceContext{ d2dDeviceContext }
{
	ZeroMemory(text, sizeof(text));
}

void UILabel::Draw()
{
	if (!isVisible) return;

    std::wostringstream outInputValue;
    outInputValue << text;

	ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, width, 24.0f, textLayout.ReleaseAndGetAddressOf()));

    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), textLayout.Get(), textBrush);
}

void UILabel::SetText(const char* arr)
{
    memcpy(&text[0], &arr[0], strlen(arr) + 1);
}

const bool UILabel::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
	case EventType::ChangeActiveLayer:
	{
		const auto derivedEvent = (ChangeActiveLayerEvent*)event;

		isVisible = false;

		if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
			isVisible = true;
		else
			isVisible = false;

		break;
	}
	}

	return false;
}