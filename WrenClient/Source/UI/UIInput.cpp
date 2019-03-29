#include "UIInput.h"
#include <wchar.h>
#include "../EventHandling/Events/MouseDownEvent.h"
#include "../EventHandling/Events/KeyDownEvent.h"
#include "../EventHandling/Events/SystemKeyDownEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Math.h"

void UIInput::Draw()
{
	if (!isVisible) return;

    // Draw Label
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), labelTextLayout, labelBrush);

    // Draw Input
    const float borderWeight = active ? 2.0f : 1.0f;
    d2dDeviceContext->FillGeometry(inputGeometry, inputBrush);
    d2dDeviceContext->DrawGeometry(inputGeometry, inputBorderBrush, borderWeight);
    
    // Draw Input Text
    std::wostringstream outInputValue;
    if (!secure)
        outInputValue << inputValue;
    else
    {
        for (auto i = 0; i < wcslen(inputValue); i++)
            outInputValue << "*";
    }
    if (active)
        outInputValue << "|";
    if (FAILED(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), inputValueTextFormat, inputWidth, height - 2, &inputValueTextLayout))) // (height - 2) takes the border into account, and looks more natural
        throw std::exception("Critical error: Failed to create the text layout for FPS information!");
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + labelWidth + 14, position.y), inputValueTextLayout, inputValueBrush);
}

const wchar_t* UIInput::GetInputValue()
{
    return inputValue;
}

void UIInput::Clear()
{
    inputIndex = 0;
    ZeroMemory(inputValue, sizeof(inputValue));
}

bool UIInput::HandleEvent(const Event& event)
{
	const auto type = event.type;
	switch (type)
	{
		case EventType::MouseDownEvent:
		{
			active = false;

			const auto mouseDownEvent = (MouseDownEvent&)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x + labelWidth, position.y, position.x + inputWidth + labelWidth, position.y + height, mouseDownEvent.mousePosX, mouseDownEvent.mousePosY))
				{
					active = true;
				}
			}

			break;
		}
		case EventType::KeyDownEvent:
		{
			if (active)
			{
				const auto keyDownEvent = (KeyDownEvent&)event;

				if (inputIndex <= 30)
				{
					inputValue[inputIndex] = keyDownEvent.charCode;
					inputIndex++;
				}
			}
			
			break;
		}
		case EventType::SystemKeyDownEvent:
		{
			if (active)
			{
				const auto keyDownEvent = (SystemKeyDownEvent&)event;
				const auto keyCode = keyDownEvent.code;

				switch (keyCode)
				{
					case SystemKey::Backspace:
					{
						if (inputIndex > 0)
						{
							inputValue[inputIndex - 1] = 0;
							inputIndex--;
						}
						break;
					}
				}
			}
			
			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent&)event;

			if (derivedEvent.layer == uiLayer)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}