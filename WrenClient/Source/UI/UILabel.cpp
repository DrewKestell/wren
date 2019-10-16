#include "stdafx.h"
#include "UILabel.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UILabel::UILabel(
	UIComponentArgs uiComponentArgs,
	const float width)
	: UIComponent(uiComponentArgs),
	  width{ width }
{
	ZeroMemory(text, sizeof(text));
}

void UILabel::Initialize(ID2D1SolidColorBrush* textBrush, IDWriteTextFormat* textFormat)
{
	this->textBrush = textBrush;
	this->textFormat = textFormat;
}

void UILabel::Draw()
{
	if (!isVisible) return;

    std::wostringstream outInputValue;
    outInputValue << text;

	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, width, 24.0f, textLayout.ReleaseAndGetAddressOf()));

    const auto position = GetWorldPosition();
    deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x, position.y), textLayout.Get(), textBrush);
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

		if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
			isVisible = true;
		else
			isVisible = false;

		break;
	}
	}

	return false;
}