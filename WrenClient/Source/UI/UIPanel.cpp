#include "stdafx.h"
#include "UIPanel.h"
#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/MouseEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../EventHandling/Events/SystemKeyDownEvent.h"
#include "../GameObject.h"
#include "../Utility.h"
#include "../Layer.h"

void UIPanel::Draw()
{
	if (!isVisible) return;

	// Draw Panel
	const float borderWeight = 2.0f;
	if (isDraggable)
	{
		d2dDeviceContext->FillGeometry(headerGeometry, headerBrush);
		d2dDeviceContext->DrawGeometry(headerGeometry, borderBrush, borderWeight);
	}
	d2dDeviceContext->FillGeometry(bodyGeometry, bodyBrush);
	d2dDeviceContext->DrawGeometry(bodyGeometry, borderBrush, borderWeight);

	// Draw Children
	const auto children = GetChildren();
	for (auto i = 0; i < children.size(); i++)
		children.at(i)->Draw();
}

bool UIPanel::HandleEvent(const Event* event)
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
				if (isVisible && isDraggable && DetectClick(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					lastDragX = mouseDownEvent->mousePosX;
					lastDragY = mouseDownEvent->mousePosY;
					isDragging = true;
				}
			}
			
			break;
		}
		case EventType::LeftMouseUpEvent:
		{
			const auto mouseUpEvent = (MouseEvent*)event;

			isDragging = false;

			break;
		}
		case EventType::MouseMoveEvent:
		{
			const auto mouseMoveEvent = (MouseEvent*)event;

			if (isDragging)
			{
				const auto deltaX = mouseMoveEvent->mousePosX - lastDragX;
				const auto deltaY = mouseMoveEvent->mousePosY - lastDragY;

				Translate(XMFLOAT3(deltaX, deltaY, 0.0f));

				lastDragX = mouseMoveEvent->mousePosX;
				lastDragY = mouseMoveEvent->mousePosY;

				const auto currentPosition = GetWorldPosition();
				if (headerGeometry != nullptr)
					headerGeometry->Release();
				if (bodyGeometry != nullptr)
					bodyGeometry->Release();
				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y + HEADER_HEIGHT, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT + height), 3.0f, 3.0f), &bodyGeometry);
			}

			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			isVisible = false;

			if (derivedEvent->layer == uiLayer)
				isActive = true;
			else
				isActive = false;

			break;
		}
		case EventType::SystemKeyDownEvent:
		{
			if (isActive)
			{
				const auto keyDownEvent = (SystemKeyDownEvent*)event;

				if (keyDownEvent->keyCode == showKey)
				{
					isVisible = !isVisible;

					auto children = GetChildren();
					for (auto i = 0; i < children.size(); i++)
					{
						auto uiComponent = (UIComponent*)children.at(i);
						uiComponent->SetVisible(!uiComponent->IsVisible());
					}
				}
			}

			break;
		}
	}

	return false;
}