#include "stdafx.h"
#include "UIButton.h"
#include "UIComponent.h"
#include "../EventHandling/Events/MouseEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../EventHandling/Observer.h"
#include "../Utility.h"
#include "../GameObject.h"
#include "../Layer.h"

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
		case EventType::LeftMouseDownEvent:
		{
			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					pressed = true;
					onClick();
				}
			}

			break;
		}
		case EventType::LeftMouseUpEvent:
		{
			const auto mouseUpEvent = (MouseEvent*)event;

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