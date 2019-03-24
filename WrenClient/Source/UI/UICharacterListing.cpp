#include "UICharacterListing.h"

bool UICharacterListing::IsSelected()
{
    return selected;
}

void UICharacterListing::SetSelected(const bool isSelected)
{
    selected = isSelected;
}

void UICharacterListing::Draw()
{
    // Draw Input
    const float borderWeight = selected ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* color = selected ? selectedBrush : brush;
    d2dDeviceContext->FillGeometry(geometry, color);
    d2dDeviceContext->DrawGeometry(geometry, borderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 10.0f, position.y + 1), textLayout, textBrush); // (location + 1) looks better
}

bool UICharacterListing::DetectClick(const float x, const float y)
{
    const auto position = GetWorldPosition();
    return x >= position.x && x <= position.x + width && y >= position.y && y <= position.y + height;
}

std::string UICharacterListing::GetCharacterName()
{
    return characterName;
}