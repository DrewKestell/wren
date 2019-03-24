#ifndef UIPANEL_H
#define UIPANEL_H

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include <vector>
#include "../GameObject.h"

const FLOAT HEADER_HEIGHT = 20.0f;

class UIPanel : public GameObject
{
    bool isVisible = false;
    bool isDragging = false;
    float lastDragX = 0.0f;
    float lastDragY = 0.0f;
    const bool isDraggable;
    float width;
    float height;
    ID2D1SolidColorBrush* headerBrush;
    ID2D1SolidColorBrush* bodyBrush;
    ID2D1SolidColorBrush* borderBrush;
    ID2D1DeviceContext1* d2dDeviceContext;
    ID2D1Factory2* d2dFactory;
    ID2D1RoundedRectangleGeometry* headerGeometry;
    ID2D1RoundedRectangleGeometry* bodyGeometry;
public:
    UIPanel(
        const DirectX::XMFLOAT3 position,
        const bool isDraggable,
        const float width,
        const float height,
        ID2D1SolidColorBrush* headerBrush,
        ID2D1SolidColorBrush* bodyBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        ID2D1Factory2* d2dFactory) :
        GameObject(position),
        isDraggable{ isDraggable },
        width{ width },
        height{ height },
        headerBrush{ headerBrush },
        bodyBrush{ bodyBrush },
        borderBrush{ borderBrush },
        d2dDeviceContext{ d2dDeviceContext },
        d2dFactory{ d2dFactory }
    {
        float startHeight = position.y;
        if (isDraggable)
        {
            d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
            startHeight += HEADER_HEIGHT;
        }
        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, startHeight, position.x + width, startHeight + height), 3.0f, 3.0f), &bodyGeometry);
    }
    bool IsVisible() { return isVisible; }
    void SetVisible(const bool isVisible) { this->isVisible = isVisible; }
    bool IsDraggable() { return isDraggable; }
    bool IsDragging() { return isDragging; }
    void StopDragging() { isDragging = false; }
    void StartDragging(const float mousePosX, const float mousePosY);
    void UpdatePosition(const float mousePosX, const float mousePosY);
    bool DetectHeaderClick(const float x, const float y);
    virtual void Draw();
};

#endif