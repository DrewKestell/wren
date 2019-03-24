#include "UILabel.h"

void UILabel::Draw()
{
    std::wostringstream outInputValue;
    outInputValue << text;
    if (FAILED(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, width, 24.0f, &textLayout)))
        throw std::exception("Critical error: Failed to create the text layout for UILabel.");
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), textLayout, textBrush);
}

void UILabel::SetText(const char* arr)
{
    memcpy(&text[0], &arr[0], strlen(arr) + 1);
}