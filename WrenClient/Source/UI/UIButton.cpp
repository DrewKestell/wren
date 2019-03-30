#include "UIButton.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/MouseUpEvent.h"
#include "../EventHandling/Events/ButtonPressEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Math.h"

void UIButton::Draw()
{
	if (!isVisible) return;

    // Draw Input
    const float borderWeight = pressed ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* buttonColor;
    if (pressed)
        buttonColor = pressedButtonBrush;
    else if (!enabled)
        buttonColor = disabledBrush;
    else
        buttonColor = buttonBrush;
    d2dDeviceContext->FillGeometry(buttonGeometry, buttonColor);
    d2dDeviceContext->DrawGeometry(buttonGeometry, buttonBorderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout, buttonTextBrush); // (location + 1) looks better
}

bool UIButton::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			const auto mouseDownEvent = (MouseDownEvent&)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
				{
					pressed = true;
					const ButtonPressEvent event{ this };
					PublishEvent(event);
					return true;
				}
			}

			break;
		}
		case EventType::MouseUpEvent:
		{
			const auto mouseUpEvent = (MouseUpEvent&)event;

			pressed = false;

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