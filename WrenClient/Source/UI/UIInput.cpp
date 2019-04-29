#include "stdafx.h"
#include "UIInput.h"
#include "UIComponent.h"
#include "../EventHandling/Observer.h"
#include "../EventHandling/Events/MouseEvent.h"
#include "../EventHandling/Events/KeyDownEvent.h"
#include "../EventHandling/Events/SystemKeyDownEvent.h"
#include "../EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Utility.h"
#include "../Layer.h"

using namespace DX;

UIInput::UIInput(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
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
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* labelTextFormat,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  secure{ secure },
	  labelWidth{ labelWidth },
	  inputWidth{ inputWidth },
	  height{ height },
	  labelBrush{ labelBrush },
	  inputBrush{ inputBrush },
	  inputBorderBrush{ inputBorderBrush },
	  inputValueBrush{ inputValueBrush },
	  inputValueTextFormat{ inputValueTextFormat },
	  writeFactory{ writeFactory },
	  d2dDeviceContext{ d2dDeviceContext }
{
	ZeroMemory(inputValue, sizeof(inputValue));

	std::wostringstream outLabelText;
	outLabelText << inLabelText;
	ThrowIfFailed(writeFactory->CreateTextLayout(outLabelText.str().c_str(), (UINT32)outLabelText.str().size(), labelTextFormat, labelWidth, height, labelTextLayout.ReleaseAndGetAddressOf()));

	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x + labelWidth + 10, position.y, position.x + labelWidth + inputWidth, position.y + height), 3.0f, 3.0f), inputGeometry.ReleaseAndGetAddressOf());
}

void UIInput::Draw()
{
	if (!isVisible) return;

    // Draw Label
    const auto position = GetWorldPosition();
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), labelTextLayout.Get(), labelBrush);

    // Draw Input
    const float borderWeight = active ? 2.0f : 1.0f;
    d2dDeviceContext->FillGeometry(inputGeometry.Get(), inputBrush);
    d2dDeviceContext->DrawGeometry(inputGeometry.Get(), inputBorderBrush, borderWeight);
    
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

	ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), inputValueTextFormat, inputWidth, height - 2, inputValueTextLayout.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
    d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + labelWidth + 14, position.y), inputValueTextLayout.Get(), inputValueBrush);
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
		case EventType::LeftMouseDownEvent:
		{
			active = false;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (DetectClick(position.x + labelWidth, position.y, position.x + inputWidth + labelWidth, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
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
				const auto keyDownEvent = (KeyDownEvent*)event;

				if (inputIndex <= 30)
				{
					inputValue[inputIndex] = keyDownEvent->charCode;
					inputIndex++;
				}
			}
			
			break;
		}
		case EventType::SystemKeyDownEvent:
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