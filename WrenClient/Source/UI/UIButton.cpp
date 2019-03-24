#include "UIButton.h"

bool UIButton::IsEnabled()
{
    return enabled;
}

void UIButton::SetEnabled(const bool isEnabled)
{
    enabled = isEnabled;
}

bool UIButton::IsPressed()
{
    return pressed;
}

void UIButton::SetPressed(const bool isPressed)
{
    pressed = isPressed;
}

void UIButton::Draw()
{
    // Draw Input
    const float borderWeight = pressed ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* buttonColor;
    if (pressed)
        buttonColor = pressedButtonBrush;
    else if (!enabled)
        buttonColor = disabledBrush;
    else
        buttonColor = buttonBrush;
    d2dDeviceContext->FillGeometry(buttonGeometry, buttonColor);
    d2dDeviceContext->DrawGeometry(buttonGeometry, buttonBorderBrush, borderWeight);
    
    // Draw Input Text
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y + 1), buttonTextLayout, buttonTextBrush); // (location + 1) looks better
}

bool UIButton::DetectClick(const float x, const float y)
{
    const auto position = GetWorldPosition();
    return x >= position.x && x <= position.x + width && y >= position.y && y <= position.y + height;
}