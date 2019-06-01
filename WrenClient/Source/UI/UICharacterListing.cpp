#include "stdafx.h"
#include "UICharacterListing.h"
#include "UIComponent.h"
#include "Layer.h"
#include "Utility.h"
#include "EventHandling/Observer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UICharacterListing::UICharacterListing(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const float width,
	const float height,
	const char* inText,
	ID2D1SolidColorBrush* brush,
	ID2D1SolidColorBrush* selectedBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* textFormat,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  width{ width },
	  height{ height },
	  brush{ brush },
	  selectedBrush{ selectedBrush },
	  borderBrush{ borderBrush },
	  textBrush{ textBrush },
	  d2dDeviceContext{ d2dDeviceContext }
{
	characterName = std::string(inText);

	std::wostringstream outText;
	outText << inText;
	ThrowIfFailed(writeFactory->CreateTextLayout(outText.str().c_str(), (UINT32)outText.str().size(), textFormat, width, height, textLayout.ReleaseAndGetAddressOf()));

	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), geometry.ReleaseAndGetAddressOf());
}

void UICharacterListing::Draw()
{
	if (!isVisible) return;

    // Draw Input
    const float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
    d2dDeviceContext->FillGeometry(geometry.Get(), color);
    d2dDeviceContext->DrawGeometry(geometry.Get(), borderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 10.0f, position.y + 1), textLayout.Get(), textBrush); // (location + 1) looks better
}

const bool UICharacterListing::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			selected = false;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
					selected = true;
			}
			
			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			selected = false;

			if (derivedEvent->layer == uiLayer)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}