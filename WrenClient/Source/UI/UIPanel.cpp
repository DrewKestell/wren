#include "UIPanel.h"
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/MouseUpEvent.h"

void UIPanel::StartDragging(const float mousePosX, const float mousePosY)
{
    lastDragX = mousePosX;
    lastDragY = mousePosY;
    isDragging = true;
}

void UIPanel::UpdatePosition(const float mousePosX, const float mousePosY)
{
    const auto deltaX = mousePosX - lastDragX;
    const auto deltaY = mousePosY - lastDragY;

    Translate(DirectX::XMFLOAT3(deltaX, deltaY, 0.0f));

    lastDragX = mousePosX;
    lastDragY = mousePosY;

    const auto currentPosition = GetWorldPosition();
    d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
    d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(currentPosition.x, currentPosition.y + HEADER_HEIGHT, currentPosition.x + width, currentPosition.y + HEADER_HEIGHT + height), 3.0f, 3.0f), &bodyGeometry);
}

bool UIPanel::DetectHeaderClick(const float x, const float y)
{
    if (!isDraggable)
        return false;

    const auto currentPosition = GetWorldPosition();
    return x >= currentPosition.x && x <= currentPosition.x + width && y >= currentPosition.y && y <= currentPosition.y + HEADER_HEIGHT;
}

void UIPanel::Draw()
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

void UIPanel::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
	case EventType::MouseDownEvent:
	{
		const auto mouseDownEvent = (MouseDownEvent&)event;

		const auto position = GetWorldPosition();
		if (DetectClick(isDraggable && position.x, position.y, position.x + width, position.y + HEADER_HEIGHT, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
		{
			// handle drag
		}

		break;
	}
	case EventType::MouseUpEvent:
	{
		const auto mouseUpEvent = (MouseUpEvent&)event;

		isDragging = false;

		break;
	}
	}
}