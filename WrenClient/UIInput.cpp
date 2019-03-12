#include "UIInput.h"

void UIInput::Draw()
{
    // Draw Label
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX, locationY), labelTextLayout, labelBrush);

    // Draw Input
    float borderWeight = active ? 3.0f : 1.0f;
    d2dDeviceContext->DrawGeometry(inputGeometry, inputBorderBrush, borderWeight);
    d2dDeviceContext->FillGeometry(inputGeometry, inputBrush);

    // Draw Input Text
    std::wostringstream outInputValue;
    outInputValue << inputValue;
    if (active)
        outInputValue << "|";
    if (FAILED(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), inputValueTextFormat, inputWidth, height, &inputvalueTextLayout)))
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(locationX + labelWidth + 10, locationY), inputvalueTextLayout, inputValueBrush);
}