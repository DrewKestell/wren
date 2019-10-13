#include "stdafx.h"
#include "UIButton.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIButton::UIButton(
	UIComponentArgs uiComponentArgs,
	const float width,
	const float height,
	const char* inButtonText,
	const std::function<void()> onClick,
	ID2D1SolidColorBrush* buttonBrush,
	ID2D1SolidColorBrush* pressedButtonBrush,
	ID2D1SolidColorBrush* buttonBorderBrush,
	ID2D1SolidColorBrush* buttonTextBrush,
	IDWriteTextFormat* buttonTextFormat)
	: UIComponent(uiComponentArgs),
	  width{ width },
	  height{ height },
	  onClick{ onClick },
	  buttonBrush{ buttonBrush },
	  pressedButtonBrush{ pressedButtonBrush },
	  buttonBorderBrush{ buttonBorderBrush },
	  buttonTextBrush{ buttonTextBrush }
{
	std::wostringstream buttonText;
	buttonText << inButtonText;
	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(
		buttonText.str().c_str(),
		(UINT32)buttonText.str().size(),
		buttonTextFormat,
		width,
		height,
		buttonTextLayout.ReleaseAndGetAddressOf())
	);
}

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

	deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), buttonGeometry.ReleaseAndGetAddressOf());

    deviceResources->GetD2DDeviceContext()->FillGeometry(buttonGeometry.Get(), buttonColor);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(buttonGeometry.Get(), buttonBorderBrush, borderWeight);
    
    // Draw Input Text    
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout.Get(), buttonTextBrush); // (location + 1) looks better
}

const bool UIButton::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if(Utility::DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					pressed = true;
					onClick();
				}
			}

			break;
		}
		case EventType::LeftMouseUp:
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