#include "UICharacterListing.h"

bool UICharacterListing::IsSelected()
{
    return selected;
}

void UICharacterListing::SetSelected(bool isSelected)
{
    selected = isSelected;
}

void UICharacterListing::Draw()
{
    // Draw Input
    float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
    d2dDeviceContext->FillGeometry(geometry, color);
    d2dDeviceContext->DrawGeometry(geometry, borderBrush, borderWeight);
    
    // Draw Input Text
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX + 10.0f, locationY + 1), textLayout, textBrush); // (location + 1) looks better
}

bool UICharacterListing::DetectClick(FLOAT x, FLOAT y)
{
    return x >= locationX && x <= locationX + width && y >= locationY && y <= locationY + height;
}

std::string UICharacterListing::GetCharacterName()
{
    return characterName;
}