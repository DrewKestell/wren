#include "UIInput.h"

void UIInput::Draw()
{
    // Draw Label
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX, locationY), labelTextLayout, labelBrush);

    // Draw Input
    float borderWeight = active ? 3.0f : 1.0f;
    d2dDeviceContext->DrawGeometry(inputGeometry, inputBorderBrush, borderWeight);
    d2dDeviceContext->FillGeometry(inputGeometry, inputBrush);
}