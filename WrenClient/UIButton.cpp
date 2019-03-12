#include "UIButton.h"

void UIButton::Draw()
{
    // Draw Label
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX, locationY), buttonTextLayout, buttonTextBrush);

    // Draw Input
    float borderWeight = pressed ? 2.0f : 1.0f;
    ID2D1SolidColorBrush* buttonColor = pressed ? pressedButtonBrush : buttonBrush;
    d2dDeviceContext->DrawGeometry(buttonGeometry, buttonBorderBrush, borderWeight);
    d2dDeviceContext->FillGeometry(buttonGeometry, buttonColor);
}