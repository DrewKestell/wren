#include "stdafx.h"
#include "UICharacterListing.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UICharacterListing::UICharacterListing(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	const float width,
	const float height,
	const char* characterName)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  width{ width },
	  height{ height },
	  characterName{ characterName }
{
	CreateGeometry();
}

void UICharacterListing::Initialize(
	ID2D1SolidColorBrush* brush,
	ID2D1SolidColorBrush* selectedBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormat
)
{
	this->brush = brush;
	this->selectedBrush = selectedBrush;
	this->borderBrush = borderBrush;
	this->textBrush = textBrush;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(characterName).c_str(),
			static_cast<unsigned int>(characterName.size()),
			textFormat,
			width,
			height,
			textLayout.ReleaseAndGetAddressOf()
		)
	);
}

void UICharacterListing::Draw()
{
	if (!isVisible) return;

    // Draw Input
    const float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
	deviceResources->GetD2DDeviceContext()->FillGeometry(geometry.Get(), color);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(geometry.Get(), borderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x + 10.0f, position.y + 1), textLayout.Get(), textBrush); // (location + 1) looks better
}

const bool UICharacterListing::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto wasSelected = selected;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
					selected = true;
			}

			if (wasSelected && !selected)
			{
				std::unique_ptr<Event> e = std::make_unique<Event>(EventType::ReorderUIComponents);
				eventHandler.QueueEvent(e);
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
		case EventType::DeselectCharacterListing:
		{
			selected = false;

			break;
		}
		case EventType::WindowResize:
		{
			CreateGeometry();
			break;
		}
	}

	return false;
}

void UICharacterListing::CreateGeometry()
{
	const auto position = GetWorldPosition();
	deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), geometry.ReleaseAndGetAddressOf());
}