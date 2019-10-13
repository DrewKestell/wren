#include "stdafx.h"
#include "UIInput.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIInput::UIInput(
	UIComponentArgs uiComponentArgs,
	const bool secure,
	const float labelWidth,
	const float inputWidth,
	const float height,
	const char* inLabelText,
	ID2D1SolidColorBrush* labelBrush,
	ID2D1SolidColorBrush* inputBrush,
	ID2D1SolidColorBrush* inputBorderBrush,
	ID2D1SolidColorBrush* inputValueBrush,
	IDWriteTextFormat* inputValueTextFormat,
	IDWriteTextFormat* labelTextFormat)
	: UIComponent(uiComponentArgs),
	  secure{ secure },
	  labelWidth{ labelWidth },
	  inputWidth{ inputWidth },
	  height{ height },
	  labelBrush{ labelBrush },
	  inputBrush{ inputBrush },
	  inputBorderBrush{ inputBorderBrush },
	  inputValueBrush{ inputValueBrush },
	  inputValueTextFormat{ inputValueTextFormat }
{
	ZeroMemory(inputValue, sizeof(inputValue));

	std::wostringstream outLabelText;
	outLabelText << inLabelText;
	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(outLabelText.str().c_str(), (UINT32)outLabelText.str().size(), labelTextFormat, labelWidth, height, labelTextLayout.ReleaseAndGetAddressOf()));

	deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(uiComponentArgs.position.x + labelWidth + 10, uiComponentArgs.position.y, uiComponentArgs.position.x + labelWidth + inputWidth, uiComponentArgs.position.y + height), 3.0f, 3.0f), inputGeometry.ReleaseAndGetAddressOf());
}

void UIInput::Draw()
{
	if (!isVisible) return;

    // Draw Label
    const auto position = GetWorldPosition();
    deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x, position.y), labelTextLayout.Get(), labelBrush);

    // Draw Input
    const float borderWeight = active ? 2.0f : 1.0f;
	deviceResources->GetD2DDeviceContext()->FillGeometry(inputGeometry.Get(), inputBrush);
	deviceResources->GetD2DDeviceContext()->DrawGeometry(inputGeometry.Get(), inputBorderBrush, borderWeight);
    
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

	ThrowIfFailed(deviceResources->GetWriteFactory()->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), inputValueTextFormat, inputWidth, height - 2, inputValueTextLayout.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
	deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(position.x + labelWidth + 14, position.y), inputValueTextLayout.Get(), inputValueBrush);
}

const wchar_t* UIInput::GetInputValue() const
{
    return inputValue;
}

const bool UIInput::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			active = false;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x + labelWidth, position.y, position.x + inputWidth + labelWidth, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					active = true;
				}
			}

			break;
		}
		case EventType::KeyDown:
		{
			if (active)
			{
				const auto keyDownEvent = (KeyDownEvent*)event;

				if (inputIndex <= 30)
				{
					inputValue[inputIndex] = keyDownEvent->charCode;
					inputIndex++;
				}
			}
			
			break;
		}
		case EventType::SystemKeyDown:
		{
			if (active)
			{
				const auto keyDownEvent = (SystemKeyDownEvent*)event;
				const auto keyCode = keyDownEvent->keyCode;

				switch (keyCode)
				{
					case VK_BACK:
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
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			active = false;
			
			// Clear input text
			inputIndex = 0;
			ZeroMemory(inputValue, sizeof(inputValue));

			if (derivedEvent->layer == uiLayer)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}