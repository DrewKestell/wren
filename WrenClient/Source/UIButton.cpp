#include "UIButton.h"

bool UIButton::IsEnabled()
{
    return enabled;
}

void UIButton::SetEnabled(bool isEnabled)
{
    enabled = isEnabled;
}

bool UIButton::IsPressed()
{
    return pressed;
}

void UIButton::SetPressed(bool isPressed)
{
    pressed = isPressed;
}

void UIButton::Draw()
{
    // Draw Input
    float borderWeight = pressed ? 2.0f : 1.0f;
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
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX, locationY + 1), buttonTextLayout, buttonTextBrush); // (location + 1) looks better
}

bool UIButton::DetectClick(FLOAT x, FLOAT y)
{
    return x >= locationX && x <= locationX + width && y >= locationY && y <= locationY + height;
}