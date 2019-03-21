#ifndef UIPANEL_H
#define UIPANEL_H

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>

const FLOAT HEADER_HEIGHT = 20.0f;

class UIPanel
{
    bool isVisible = false;
    bool isDragging = false;
    FLOAT lastDragX = 0.0f;
    FLOAT lastDragY = 0.0f;
    bool isDraggable;
    FLOAT locationX;
    FLOAT locationY;
    FLOAT width;
    FLOAT height;
    ID2D1SolidColorBrush* headerBrush;
    ID2D1SolidColorBrush* bodyBrush;
    ID2D1SolidColorBrush* borderBrush;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteFactory2* writeFactory;
    ID2D1Factory2* d2dFactory;
    ID2D1RoundedRectangleGeometry* headerGeometry;
    ID2D1RoundedRectangleGeometry* bodyGeometry;
public:
    UIPanel(
        const bool isDraggable,
        const FLOAT locationX,
        const FLOAT locationY,
        const FLOAT width,
        const FLOAT height,
        ID2D1SolidColorBrush* headerBrush,
        ID2D1SolidColorBrush* bodyBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        IDWriteFactory2* writeFactory,
        ID2D1Factory2* d2dFactory) :
        isDraggable{ isDraggable },
        locationX{ locationX },
        locationY{ locationY },
        width{ width },
        height{ height },
        headerBrush{ headerBrush },
        bodyBrush{ bodyBrush },
        borderBrush{ borderBrush },
        d2dDeviceContext{ d2dDeviceContext },
        writeFactory{ writeFactory },
        d2dFactory{ d2dFactory }
    {
        FLOAT startHeight = locationY;
        if (isDraggable)
        {
            d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, locationY, locationX + width, locationY + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
            startHeight += HEADER_HEIGHT;
        }
        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, startHeight, locationX + width, startHeight + height), 3.0f, 3.0f), &bodyGeometry);
    }
    
    bool IsVisible() { return isVisible; }
    void SetVisible(bool isVisible) { this->isVisible = isVisible; }
    bool IsDraggable() { return isDraggable; }
    bool IsDragging() { return isDragging; }
    void StopDragging() { isDragging = false; }
    void StartDragging(FLOAT mousePosX, FLOAT mousePosY);
    void UpdatePosition(FLOAT mousePosX, FLOAT mousePosY);
    bool DetectHeaderClick(FLOAT x, FLOAT y);
    void Draw();
};

#endif