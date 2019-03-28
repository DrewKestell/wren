#include "UIPanel.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/MouseUpEvent.h"
#include "../EventHandling/Events/MouseMoveEvent.h"

void UIPanel::Draw(const Layer layer)
{
	if (uiLayer & layer & Any == 0)
		return;

	if (isVisible)
	{
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
}

void UIPanel::HandleEvent(const Event& event, const Layer layer)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			if (uiLayer & layer & Any == 0)
				break;

			const auto mouseDownEvent = (MouseDownEvent&)event;

			const auto position = GetWorldPosition();
			if (isVisible && isDraggable && DetectClick(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
			{
				lastDragX = mouseDownEvent.mousePosX;
				lastDragY = mouseDownEvent.mousePosY;
				isDragging = true;
			}

			break;
		}
		case EventType::MouseUpEvent:
		{
			const auto mouseUpEvent = (MouseUpEvent&)event;

			isDragging = false;

			break;
		}
		case EventType::MouseMoveEvent:
		{
			if (uiLayer & layer & Any == 0)
				break;

			const auto mouseMoveEvent = (MouseMoveEvent&)event;

			if (isDragging)
			{
				const auto deltaX = mouseMoveEvent.mousePosX - lastDragX;
				const auto deltaY = mouseMoveEvent.mousePosY - lastDragY;

				Translate(DirectX::XMFLOAT3(deltaX, deltaY, 0.0f));

				lastDragX = mouseMoveEvent.mousePosX;
				lastDragY = mouseMoveEvent.mousePosY;

				const auto currentPosition = GetWorldPosition();
				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
				d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y + HEADER_HEIGHT, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT + height), 3.0f, 3.0f), &bodyGeometry);
			}
		}
	}
}