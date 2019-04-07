#include "stdafx.h"
#include "UILabel.h"
#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../GameObject.h"
#include "../Layer.h"

void UILabel::Draw()
{
	if (!isVisible) return;

    std::wostringstream outInputValue;
    outInputValue << text;
	if (textLayout != nullptr)
		textLayout->Release();
    if (FAILED(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, width, 24.0f, &textLayout)))
        throw std::exception("Critical error: Failed to create the text layout for UILabel.");
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), textLayout, textBrush);
}

void UILabel::SetText(const char* arr)
{
    memcpy(&text[0], &arr[0], strlen(arr) + 1);
}

bool UILabel::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
	case EventType::ChangeActiveLayer:
	{
		const auto derivedEvent = (ChangeActiveLayerEvent*)event;

		isVisible = false;

		if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
			isVisible = true;
		else
			isVisible = false;

		break;
	}
	}

	return false;
}