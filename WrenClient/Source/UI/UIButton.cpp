#include "stdafx.h"
#include "UIButton.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "Utility.h"
#include "Layer.h"

using namespace DX;

UIButton::UIButton(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const float width,
	const float height,
	const char* inButtonText,
	const std::function<void()> onClick,
	ID2D1SolidColorBrush* buttonBrush,
	ID2D1SolidColorBrush* pressedButtonBrush,
	ID2D1SolidColorBrush* buttonBorderBrush,
	ID2D1SolidColorBrush* buttonTextBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* buttonTextFormat,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  width{ width },
	  height{ height },
	  onClick{ onClick },
	  buttonBrush{ buttonBrush },
	  pressedButtonBrush{ pressedButtonBrush },
	  buttonBorderBrush{ buttonBorderBrush },
	  buttonTextBrush{ buttonTextBrush },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory }
{
	std::wostringstream buttonText;
	buttonText << inButtonText;
	ThrowIfFailed(writeFactory->CreateTextLayout(
		buttonText.str().c_str(),
		(UINT32)buttonText.str().size(),
		buttonTextFormat,
		width,
		height,
		buttonTextLayout.ReleaseAndGetAddressOf())
	);

	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), buttonGeometry.ReleaseAndGetAddressOf());
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

    d2dDeviceContext->FillGeometry(buttonGeometry.Get(), buttonColor);
    d2dDeviceContext->DrawGeometry(buttonGeometry.Get(), buttonBorderBrush, borderWeight);
    
    // Draw Input Text    
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout.Get(), buttonTextBrush); // (location + 1) looks better
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
				if (Utility::DetectClick(position.x, position.y, position.x + width, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
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