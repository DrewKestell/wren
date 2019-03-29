#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include <vector>
#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Publisher.h"

const FLOAT HEADER_HEIGHT = 20.0f;

class UIPanel : public UIComponent, public Observer, public Publisher
{
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
		ObjectManager& objectManager,
		const Layer uiLayer,
		EventHandler& eventHandler,
        const bool isDraggable,
        const float width,
        const float height,
        ID2D1SolidColorBrush* headerBrush,
        ID2D1SolidColorBrush* bodyBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        ID2D1Factory2* d2dFactory) :
        UIComponent(objectManager, position, uiLayer),
		Observer(eventHandler),
		Publisher(eventHandler),
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
    
    virtual void Draw();
	virtual bool HandleEvent(const Event& event);
};
