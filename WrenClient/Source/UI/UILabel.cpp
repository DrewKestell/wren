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
}

void UILabel::Initialize(ID2D1SolidColorBrush* textBrush, IDWriteTextFormat* textFormat)
{
	this->textBrush = textBrush;
	this->textFormat = textFormat;

	CreateTextLayout();
}

void UILabel::Draw()
{
	if (!isVisible) return;

    const auto position = GetWorldPosition();
    deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x, position.y), textLayout.Get(), textBrush);
}

void UILabel::SetText(const char* text)
{
	this->text = text;

	CreateTextLayout();
}

void UILabel::CreateTextLayout()
{
	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(this->text).c_str(),
			static_cast<unsigned int>(this->text.size()),
			textFormat,
			width,
			24.0f,
			textLayout.ReleaseAndGetAddressOf()
		)
	);
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