#include "UICharacterListing.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/SelectCharacterListing.h"
#include "../EventHandling/Events/DeselectCharacterListing.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"

void UICharacterListing::Draw()
{
	if (!isVisible) return;

    // Draw Input
    const float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
    d2dDeviceContext->FillGeometry(geometry, color);
    d2dDeviceContext->DrawGeometry(geometry, borderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 10.0f, position.y + 1), textLayout, textBrush); // (location + 1) looks better
}

bool UICharacterListing::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			selected = false;

			const auto mouseDownEvent = (MouseDownEvent&)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
				{
					selected = true;
					PublishEvent(SelectCharacterListing{ &characterName });
				}
				else
				{
					PublishEvent(DeselectCharacterListing{});
				}
			}
			
			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent&)event;

			if (derivedEvent.layer == uiLayer)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}