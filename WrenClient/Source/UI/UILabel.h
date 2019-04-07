#pragma once

#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/Event.h"

class UILabel : public UIComponent, public Observer
{
    char text[200];
    float width;
    ID2D1SolidColorBrush* textBrush = nullptr;
    IDWriteTextFormat* textFormat = nullptr;
    IDWriteFactory2* writeFactory = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    IDWriteTextLayout* textLayout = nullptr;
public:
    UILabel(
        const XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
        const float width,
        ID2D1SolidColorBrush* textBrush,
        IDWriteTextFormat* textFormat,
        ID2D1DeviceContext1* d2dDeviceContext,
        IDWriteFactory2* writeFactory,
        ID2D1Factory2* d2dFactory) :
        UIComponent(objectManager, position, uiLayer),
        width{ width },
        textBrush{ textBrush },
        textFormat{ textFormat },
        writeFactory{ writeFactory },
        d2dDeviceContext{ d2dDeviceContext }
    {
        ZeroMemory(text, sizeof(text));
    }
    virtual void Draw();
	virtual bool HandleEvent(const Event* event);
    void SetText(const char* arr);
};