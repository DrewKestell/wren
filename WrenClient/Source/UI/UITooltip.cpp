#include "stdafx.h"
#include "UITooltip.h"

UITooltip::UITooltip(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	EventHandler& eventHandler,
	const std::string& headerText,
	const std::string& bodyText,
	ID2D1SolidColorBrush* bodyBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormatTitle,
	IDWriteTextFormat* textFormatDescription,
	IDWriteFactory2* writeFactory,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex, false),
	  eventHandler{ eventHandler },
	  bodyBrush{ bodyBrush },
	  borderBrush{ borderBrush },
	  textBrush{ textBrush },
	  textFormatTitle{ textFormatTitle },
	  textFormatDescription{ textFormatDescription },
	  writeFactory{ writeFactory },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory }
{
	writeFactory->CreateTextLayout(Utility::s2ws(headerText).c_str(), headerText.size(), textFormatTitle, 300.0f, 0.0f, headerTextLayout.ReleaseAndGetAddressOf());
	writeFactory->CreateTextLayout(Utility::s2ws(bodyText).c_str(), bodyText.size(), textFormatDescription, 300.0f, 0.0f, bodyTextLayout.ReleaseAndGetAddressOf());
}

void UITooltip::Draw()
{
	if (!isVisible) return;

	const auto pos = GetWorldPosition();

	DWRITE_TEXT_METRICS textMetrics;
	bodyTextLayout->GetMetrics(&textMetrics);
	const auto width = textMetrics.width + 16.0f;
	const auto height = textMetrics.height + 36.0f;

	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(pos.x, pos.y, pos.x + width, pos.y + height), 3.0f, 3.0f), bodyGeometry.ReleaseAndGetAddressOf());

	d2dDeviceContext->FillGeometry(bodyGeometry.Get(), bodyBrush);
	d2dDeviceContext->DrawGeometry(bodyGeometry.Get(), borderBrush, 2.0f);

	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(pos.x + 8.0f, pos.y + 8.0f), headerTextLayout.Get(), textBrush);
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(pos.x + 8.0f, pos.y + 28.0f), bodyTextLayout.Get(), textBrush);
}

const bool UITooltip::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		
	}
	return false;
}