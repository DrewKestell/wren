#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include "../GameObject.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Publisher.h"

class UICharacterListing : public GameObject, public Observer, public Publisher
{
    std::string characterName;
    bool selected;
    float width;
    float height;
    ID2D1SolidColorBrush* brush;
    ID2D1SolidColorBrush* selectedBrush;
    ID2D1SolidColorBrush* borderBrush;
    ID2D1SolidColorBrush* textBrush;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* textLayout;
    ID2D1RoundedRectangleGeometry* geometry;
public:
    UICharacterListing(
        const DirectX::XMFLOAT3 position,
		EventHandler* eventHandler,
        const float width,
        const float height,
        ID2D1SolidColorBrush* brush,
        ID2D1SolidColorBrush* selectedBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1SolidColorBrush* textBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* textFormat,
        ID2D1Factory2* d2dFactory) :
        GameObject{ position },
		Observer(eventHandler),
		Publisher(eventHandler),
        width{ width },
        height{ height },
        brush{ brush },
        selectedBrush{ selectedBrush },
        borderBrush{ borderBrush },
        textBrush{ textBrush },
        d2dDeviceContext{ d2dDeviceContext }
    {
        characterName = std::string(inText);

        std::wostringstream outText;
        outText << inText;
        if (FAILED(writeFactory->CreateTextLayout(outText.str().c_str(), (UINT32)outText.str().size(), textFormat, width, height, &textLayout)))
            throw std::exception("Failed to create text layout for UICharacterListing.");

        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x, position.y, position.x + width, position.y + height), 3.0f, 3.0f), &geometry);

    }
    virtual void Draw();
	virtual void HandleEvent(const Event& event);
    std::string& GetCharacterName();
};