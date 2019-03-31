#include "UIButton.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/MouseUpEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Utility.h"

void UIButton::Draw()
{
	if (!isVisible) return;

	const auto position = GetWorldPosition();

    // Draw Input
    const float borderWeight = pressed ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* buttonColor;
    if (pressed)
        buttonColor = pressedButtonBrush;
    else if (!enabled)
        buttonColor = disabledBrush;
    else
        buttonColor = buttonBrush;
	if (buttonGeometry != nullptr)
		buttonGeometry->Release();
	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), &buttonGeometry);
    d2dDeviceContext->FillGeometry(buttonGeometry, buttonColor);
    d2dDeviceContext->DrawGeometry(buttonGeometry, buttonBorderBrush, borderWeight);
    
    // Draw Input Text    
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout, buttonTextBrush); // (location + 1) looks better
}

bool UIButton::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			const auto mouseDownEvent = (MouseDownEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					pressed = true;
					onClick();
					return true;
				}
			}

			break;
		}
		case EventType::MouseUpEvent:
		{
			const auto mouseUpEvent = (MouseUpEvent*)event;

			pressed = false;

			break;
		}
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