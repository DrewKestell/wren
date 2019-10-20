#include "stdafx.h"
#include "UITooltip.h"

UITooltip::UITooltip(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	const std::string& headerText,
	const std::string& bodyText)
	: UIComponent(uiComponentArgs, false, false),
	  eventHandler{ eventHandler },
	  headerText{ headerText },
	  bodyText{ bodyText }
{
}

void UITooltip::Initialize(
	ID2D1SolidColorBrush* bodyBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormatTitle,
	IDWriteTextFormat* textFormatDescription)
{
	this->bodyBrush = bodyBrush;
	this->borderBrush = borderBrush;
	this->textBrush = textBrush;
	this->textFormatTitle = textFormatTitle;
	this->textFormatDescription = textFormatDescription;

	deviceResources->GetWriteFactory()->CreateTextLayout(Utility::s2ws(headerText).c_str(), headerText.size(), textFormatTitle, 300.0f, 0.0f, headerTextLayout.ReleaseAndGetAddressOf());
	deviceResources->GetWriteFactory()->CreateTextLayout(Utility::s2ws(bodyText).c_str(), bodyText.size(), textFormatDescription, 300.0f, 0.0f, bodyTextLayout.ReleaseAndGetAddressOf());

	CreateGeometry();
}

void UITooltip::Draw()
{
	if (!isVisible) return;

	deviceResources->GetD2DDeviceContext()->FillGeometry(bodyGeometry.Get(), bodyBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(bodyGeometry.Get(), borderBrush, 2.0f);

	const auto pos = GetWorldPosition();
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(pos.x + 8.0f, pos.y + 8.0f), headerTextLayout.Get(), textBrush);
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(pos.x + 8.0f, pos.y + 28.0f), bodyTextLayout.Get(), textBrush);
}

const bool UITooltip::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

	const auto type = event->type;
	switch (type)
	{
		case EventType::WindowResize:
		{
			CreateGeometry();

			break;
		}
	}
	return false;
} 

void UITooltip::CreateGeometry()
{
	DWRITE_TEXT_METRICS textMetrics;
	bodyTextLayout->GetMetrics(&textMetrics);
	const auto width = textMetrics.width + 16.0f;
	const auto height = textMetrics.height + 36.0f;

	const auto pos = GetWorldPosition();
	deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(pos.x, pos.y, pos.x + width, pos.y + height), 3.0f, 3.0f), bodyGeometry.ReleaseAndGetAddressOf());
}