#include "UICharacterListing.h"
#include "../EventHandling/Events/MouseDownEvent.h"

void UICharacterListing::Draw()
{
    // Draw Input
    const float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
    d2dDeviceContext->FillGeometry(geometry, color);
    d2dDeviceContext->DrawGeometry(geometry, borderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 10.0f, position.y + 1), textLayout, textBrush); // (location + 1) looks better
}

std::string& UICharacterListing::GetCharacterName()
{
    return characterName;
}

void UICharacterListing::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			selected = false;

			const auto mouseDownEvent = (MouseDownEvent&)event;

			const auto position = GetWorldPosition();
			if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
			{
				selected = true;
			}

			break;
		}
	}
}