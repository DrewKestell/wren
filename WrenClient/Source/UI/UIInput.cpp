#include "stdafx.h"
#include "UIInput.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include <sstream>
#include <iomanip>

using namespace DX;

extern bool g_leftCtrlHeld;
extern bool g_leftShiftHeld;

UIInput::UIInput(
	UIComponentArgs uiComponentArgs,
	const bool secure,
	const float labelWidth,
	const float inputWidth,
	const float height,
	const char* labelText,
	const UIInputType inputType)
	: UIComponent(uiComponentArgs),
	  secure{ secure },
	  labelWidth{ labelWidth },
	  inputWidth{ inputWidth },
	  height{ height },
	  labelText{ labelText },
	  inputType{ inputType }
{
}

void UIInput::Initialize(
	ID2D1SolidColorBrush* labelBrush,
	ID2D1SolidColorBrush* inputBrush,
	ID2D1SolidColorBrush* inputBorderBrush,
	ID2D1SolidColorBrush* inputValueBrush,
	IDWriteTextFormat* inputValueTextFormat,
	IDWriteTextFormat* labelTextFormat)
{
	this->labelBrush = labelBrush;
	this->inputBrush = inputBrush;
	this->inputBorderBrush = inputBorderBrush;
	this->inputValueBrush = inputValueBrush;
	this->inputValueTextFormat = inputValueTextFormat;
	this->labelTextFormat = labelTextFormat;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(labelText).c_str(),
			static_cast<unsigned int>(labelText.size()),
			labelTextFormat,
			labelWidth,
			height,
			labelTextLayout.ReleaseAndGetAddressOf()
		)
	);
}

void UIInput::Draw()
{
	if (!isVisible) return;

    // Draw Label
    const auto position = GetWorldPosition();
	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x, position.y), labelTextLayout.Get(), labelBrush);

    // Draw Input
    const float borderWeight = active ? 2.0f : 1.0f;
	d2dDeviceContext->FillGeometry(inputGeometry.Get(), inputBrush);
	d2dDeviceContext->DrawGeometry(inputGeometry.Get(), inputBorderBrush, borderWeight);
    
    // Draw Input Text   
	if (inputIndex > 0)
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + labelWidth + 14.0f, position.y + 1.0f), inputValueTextLayout.Get(), inputValueBrush); // +1.0f in the y direction looks more centered
}

const bool UIInput::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto wasActive = active;
			active = false;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x + labelWidth, position.y, position.x + inputWidth + labelWidth, position.y + height, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
					active = true;

				if(wasActive != active)
					CreateTextLayout();
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

					CreateTextLayout();
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

							CreateTextLayout();
						}
						break;
					}
					case VK_UP:
					{
						if (inputType == UIInputType::Number)
						{
							if (g_leftShiftHeld)
								IncrementValue(1);
							else if (g_leftCtrlHeld)
								IncrementValue(0.01f);
							else
								IncrementValue(0.1f);
						}

						break;
					}
					case VK_DOWN:
					{
						if (inputType == UIInputType::Number)
						{
							if (g_leftShiftHeld)
								IncrementValue(-1);
							else if (g_leftCtrlHeld)
								IncrementValue(-0.01f);
							else
								IncrementValue(-0.1f);
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

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
		case EventType::WindowResize:
		{
			const auto position = GetWorldPosition();
			deviceResources->GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(position.x + labelWidth + 10, position.y, position.x + labelWidth + inputWidth, position.y + height), 3.0f, 3.0f), inputGeometry.ReleaseAndGetAddressOf());
		}
	}

	return false;
}

const wchar_t* UIInput::GetInputValue() const
{
	return inputValue;
}

void UIInput::SetInputValue(const wchar_t* val, const int len)
{
	ZeroMemory(inputValue, sizeof(inputValue));
	memcpy(inputValue, val, sizeof(wchar_t) * len);
	inputIndex = len;
	CreateTextLayout();
}

void UIInput::ClearInput()
{
	active = false;

	inputIndex = 0;
	ZeroMemory(inputValue, sizeof(inputValue));
}

void UIInput::CreateTextLayout()
{
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

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			outInputValue.str().c_str(),
			(UINT32)outInputValue.str().size(),
			inputValueTextFormat,
			inputWidth,
			height - 2, // (height - 2) takes the border into account, and looks more natural
			inputValueTextLayout.ReleaseAndGetAddressOf()
		)
	);
}

void UIInput::IncrementValue(const float amount)
{
	try
	{
		auto val = std::wcstof(inputValue, nullptr);
		val += amount;

		std::wstringstream out;
		out << val;
		auto newVal = std::wstring(out.str());

		SetInputValue(newVal.data(), newVal.length());
	}
	catch (std::exception ex)
	{
		// fail silently?
	}
}
