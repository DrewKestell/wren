#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>
#include "UIComponent.h"
#include "../EventHandling/Observer.h"

class UICharacterListing : public UIComponent, public Observer
{
    std::string characterName;
    bool selected;
    float width;
    float height;
    ID2D1SolidColorBrush* brush = nullptr;
    ID2D1SolidColorBrush* selectedBrush = nullptr;
    ID2D1SolidColorBrush* borderBrush = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
    IDWriteTextLayout* textLayout = nullptr;
    ID2D1RoundedRectangleGeometry* geometry = nullptr;
public:
    UICharacterListing(
        const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const Layer uiLayer,
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
        UIComponent{ objectManager, position, uiLayer },
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
	virtual bool HandleEvent(const Event* event);
	std::string& GetName() { return characterName; }
	bool IsSelected() { return selected; }
	~UICharacterListing()
	{
		textLayout->Release();
		geometry->Release();
	}
};