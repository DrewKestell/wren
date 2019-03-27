#include "UIButton.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/MouseUpEvent.h"
#include "../EventHandling/Events/ButtonPressEvent.h"
#include "../Math.h"

bool UIButton::IsEnabled()
{
    return enabled;
}

void UIButton::SetEnabled(const bool isEnabled)
{
    enabled = isEnabled;
}

bool UIButton::IsPressed()
{
    return pressed;
}

void UIButton::Draw()
{
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

void UIButton::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			const auto mouseDownEvent = (MouseDownEvent&)event;

			const auto position = GetWorldPosition();
			if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
			{
				pressed = true;
				const ButtonPressEvent event{ buttonId };
				PublishEvent(event);
			}

			break;
		}
		case EventType::MouseUpEvent:
		{
			const auto mouseUpEvent = (MouseUpEvent&)event;

			pressed = false;

			break;
		}
	}
}