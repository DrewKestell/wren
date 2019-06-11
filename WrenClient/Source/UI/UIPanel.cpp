#include "stdafx.h"
#include "UIPanel.h"
#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "Utility.h"
#include "Layer.h"

UIPanel::UIPanel(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const int zIndex,
	const bool isDraggable,
	const float width,
	const float height,
	const WPARAM showKey,
	ID2D1SolidColorBrush* headerBrush,
	ID2D1SolidColorBrush* bodyBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  isDraggable{ isDraggable },
	  width{ width },
	  height{ height },
	  showKey{ showKey },
	  headerBrush{ headerBrush },
	  bodyBrush{ bodyBrush },
	  borderBrush{ borderBrush },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory }
{
	float startHeight = position.y;
	if (isDraggable)
	{
		d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT), 3.0f, 3.0f), headerGeometry.ReleaseAndGetAddressOf());
		startHeight += HEADER_HEIGHT;
	}
	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, startHeight, position.x + width, startHeight + height), 3.0f, 3.0f), bodyGeometry.ReleaseAndGetAddressOf());
}

void UIPanel::Draw()
{
	if (!isVisible) return;

	// Draw Panel
	const float borderWeight = 2.0f;
	if (isDraggable)
	{
		d2dDeviceContext->FillGeometry(headerGeometry.Get(), headerBrush);
		d2dDeviceContext->DrawGeometry(headerGeometry.Get(), borderBrush, borderWeight);
	}
	d2dDeviceContext->FillGeometry(bodyGeometry.Get(), bodyBrush);
	d2dDeviceContext->DrawGeometry(bodyGeometry.Get(), borderBrush, borderWeight);
}

const bool UIPanel::HandleEvent(const Event* const event)
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
				if (isVisible && isDraggable && Utility::DetectClick(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					lastDragX = mouseDownEvent->mousePosX;
					lastDragY = mouseDownEvent->mousePosY;
					isDragging = true;
				}
			}
			
			break;
		}
		case EventType::LeftMouseUp:
		{
			const auto mouseUpEvent = (MouseEvent*)event;

			isDragging = false;

			break;
		}
		case EventType::MouseMove:
		{
			const auto mouseMoveEvent = (MouseEvent*)event;

			if (isDragging)
			{
				const auto deltaX = mouseMoveEvent->mousePosX - lastDragX;
				const auto deltaY = mouseMoveEvent->mousePosY - lastDragY;

				Translate(XMFLOAT2{ deltaX, deltaY });

				lastDragX = mouseMoveEvent->mousePosX;
				lastDragY = mouseMoveEvent->mousePosY;

				const auto currentPosition = GetWorldPosition();

				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT), 3.0f, 3.0f), headerGeometry.ReleaseAndGetAddressOf());
				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y + HEADER_HEIGHT, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT + height), 3.0f, 3.0f), bodyGeometry.ReleaseAndGetAddressOf());
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
		case EventType::SystemKeyDown:
		{
			if (isActive)
			{
				const auto keyDownEvent = (SystemKeyDownEvent*)event;

				if (keyDownEvent->keyCode == showKey)
				{
					isVisible = !isVisible;

					SetChildrenAsVisible(this);
				}
			}

			break;
		}
	}

	return false;
}

void UIPanel::SetChildrenAsVisible(UIComponent* uiComponent)
{
	auto children = uiComponent->GetChildren();
	for (auto i = 0; i < children.size(); i++)
	{
		auto uiComponent = (UIComponent*)children.at(i);
		uiComponent->SetVisible(!uiComponent->IsVisible());
		SetChildrenAsVisible(uiComponent);
	}
}