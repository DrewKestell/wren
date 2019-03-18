#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <sstream>

class UICharacterListing
{
    bool selected;
    FLOAT locationX;
    FLOAT locationY;
    FLOAT width;
    FLOAT height;
    ID2D1SolidColorBrush* brush;
    ID2D1SolidColorBrush* selectedBrush;
    ID2D1SolidColorBrush* borderBrush;
    ID2D1SolidColorBrush* textBrush;
    ID2D1DeviceContext1* d2dDeviceContext;
    IDWriteTextLayout* textLayout;
    ID2D1RoundedRectangleGeometry* geometry;
public:
    UICharacterListing(
        const FLOAT locationX,
        const FLOAT locationY,
        const FLOAT width,
        const FLOAT height,
        ID2D1SolidColorBrush* brush,
        ID2D1SolidColorBrush* selectedBrush,
        ID2D1SolidColorBrush* borderBrush,
        ID2D1SolidColorBrush* textBrush,
        ID2D1DeviceContext1* d2dDeviceContext,
        const char* inText,
        IDWriteFactory2* writeFactory,
        IDWriteTextFormat* textFormat,
        ID2D1Factory2* d2dFactory) :
        locationX{ locationX },
        locationY{ locationY },
        width{ width },
        height{ height },
        brush{ brush },
        selectedBrush{ selectedBrush },
        borderBrush{ borderBrush },
        textBrush{ textBrush },
        d2dDeviceContext{ d2dDeviceContext }
    {
        std::wostringstream outText;
        outText << inText;
        if (FAILED(writeFactory->CreateTextLayout(outText.str().c_str(), (UINT32)outText.str().size(), textFormat, width, height, &textLayout)))
            throw std::exception("Failed to create text layout for UICharacterListing.");

        d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(locationX, locationY, locationX + width, locationY + height), 3.0f, 3.0f), &geometry);

    }
    bool IsSelected();
    void SetSelected(bool isPressed);
    void Draw();
    bool DetectClick(FLOAT x, FLOAT y);
};