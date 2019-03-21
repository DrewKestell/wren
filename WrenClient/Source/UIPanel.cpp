#include "UIPanel.h"

void UIPanel::StartDragging(FLOAT mousePosX, FLOAT mousePosY)
{
    lastDragX = mousePosX;
    lastDragY = mousePosY;
    isDragging = true;
}

void UIPanel::UpdatePosition(FLOAT mousePosX, FLOAT mousePosY)
{
    locationX = (mousePosX - lastDragX);
    locationY = (mousePosY - lastDragY);
    lastDragX = mousePosX;
    lastDragY = mousePosY;

    d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, locationY, locationX + width, locationY + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
    d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, HEADER_HEIGHT, locationX + width, HEADER_HEIGHT + height), 3.0f, 3.0f), &bodyGeometry);
}

bool UIPanel::DetectHeaderClick(FLOAT x, FLOAT y)
{
    if (isDraggable)
        return false;
    bool isWithinX = x >= locationX && x <= locationX + width;
    bool isWithinY = y >= locationY && y <= locationY + HEADER_HEIGHT;
    bool both = isWithinX && isWithinY;
    return both;
}

void UIPanel::Draw()
{
    // Draw Input
    const float borderWeight = 2.0f;
    if (isDraggable)
    {
        d2dDeviceContext->FillGeometry(headerGeometry, headerBrush);
        d2dDeviceContext->DrawGeometry(headerGeometry, borderBrush, borderWeight);
    }
    d2dDeviceContext->FillGeometry(bodyGeometry, bodyBrush);
    d2dDeviceContext->DrawGeometry(bodyGeometry, borderBrush, borderWeight);
}