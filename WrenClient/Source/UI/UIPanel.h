#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include <vector>
#include "UIComponent.h"
#include "../EventHandling/Observer.h"

const FLOAT HEADER_HEIGHT = 20.0f;

class UIPanel : public UIComponent, public Observer
{
	bool isActive = false;
    bool isDragging = false;
    float lastDragX = 0.0f;
    float lastDragY = 0.0f;
    const bool isDraggable;
    float width;
    float height;
	WPARAM showKey;
    ID2D1SolidColorBrush* headerBrush = nullptr;
    ID2D1SolidColorBrush* bodyBrush = nullptr;
    ID2D1SolidColorBrush* borderBrush = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    ID2D1Factory2* d2dFactory = nullptr;
    ID2D1RoundedRectangleGeometry* headerGeometry = nullptr;
    ID2D1RoundedRectangleGeometry* bodyGeometry = nullptr;
public:
    UIPanel(
        const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
        const bool isDraggable,
        const float width,
        const float height,
		const WPARAM showKey,
        ID2D1SolidColorBrush* headerBrush,
        ID2D1SolidColorBrush* bodyBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        ID2D1Factory2* d2dFactory) :
        UIComponent(objectManager, position, uiLayer),
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
            d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + HEADER_HEIGHT), 3.0f, 3.0f), &headerGeometry);
            startHeight += HEADER_HEIGHT;
        }
        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, startHeight, position.x + width, startHeight + height), 3.0f, 3.0f), &bodyGeometry);
    }
    
    virtual void Draw();
	virtual bool HandleEvent(const Event* event);
};
