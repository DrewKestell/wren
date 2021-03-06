#include "stdafx.h"
#include "UIButton.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIButton::UIButton(
	UIComponentArgs uiComponentArgs,
	const float width,
	const float height,
	const char* buttonText,
	const std::function<void()> onClick)
	: UIComponent(uiComponentArgs),
	  width{ width },
	  height{ height },
	  buttonText{ buttonText },
	  onClick{ onClick }
{
}

void UIButton::Initialize(
	ID2D1SolidColorBrush* buttonBrush,
	ID2D1SolidColorBrush* pressedButtonBrush,
	ID2D1SolidColorBrush* buttonBorderBrush,
	ID2D1SolidColorBrush* buttonTextBrush,
	IDWriteTextFormat* buttonTextFormat)
{
	this->buttonBrush = buttonBrush;
	this->pressedButtonBrush = pressedButtonBrush;
	this->buttonBorderBrush = buttonBorderBrush;
	this->buttonTextBrush = buttonTextBrush;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(buttonText).c_str(),
			static_cast<unsigned int>(buttonText.size()),
			buttonTextFormat,
			width,
			height,
			buttonTextLayout.ReleaseAndGetAddressOf()
		)
	);
}

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

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();

	d2dDeviceContext->FillGeometry(buttonGeometry.Get(), buttonColor);
	d2dDeviceContext->DrawGeometry(buttonGeometry.Get(), buttonBorderBrush, borderWeight);
    
    // Draw Input Text   
	const auto position = GetWorldPosition();
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout.Get(), buttonTextBrush); // (location + 1) looks better
}

const bool UIButton::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

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
		case EventType::WindowResize:
		{
			const auto position = GetWorldPosition();
			deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), buttonGeometry.ReleaseAndGetAddressOf());
		
			break;
		}
	}

	return false;
}