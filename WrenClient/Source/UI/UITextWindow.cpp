#include "stdafx.h"
#include "Layer.h"
#include "UITextWindow.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/SystemKeyDownEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/SendChatMessage.h"

using namespace DX;

extern EventHandler g_eventHandler;

UITextWindow::UITextWindow(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	std::vector<std::string*>* messages,
	ID2D1SolidColorBrush* backgroundBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* inputBrush,
	ID2D1SolidColorBrush* inputTextBrush,
	ID2D1SolidColorBrush* inputTextBrushInactive,
	ID2D1SolidColorBrush* textBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* textFormat,
	IDWriteTextFormat* textFormatInactive,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	messages{ messages },
	backgroundBrush{ backgroundBrush },
	borderBrush{ borderBrush },
	inputBrush{ inputBrush },
	inputTextBrush{ inputTextBrush },
	inputTextBrushInactive{ inputTextBrushInactive },
	textBrush{ textBrush },
	d2dDeviceContext{ d2dDeviceContext },
	writeFactory{ writeFactory },
	textFormat{ textFormat },
	textFormatInactive{ textFormatInactive },
	d2dFactory{ d2dFactory }
{
	UpdateMessages();

	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + 600.0f, position.y + 220.0f), windowGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y + 220.0f, position.x + 600.0f, position.y + 245.0f), inputGeometry.ReleaseAndGetAddressOf());

	std::wostringstream outInputValue;
	outInputValue << "Enter a message...";
	ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormatInactive, 600.0f, 25.0f, inputValueTextLayoutInactive.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
}

void UITextWindow::Draw()
{
	if (!isVisible) return;

	// Draw window
	d2dDeviceContext->FillGeometry(windowGeometry.Get(), backgroundBrush);
	d2dDeviceContext->FillGeometry(inputGeometry.Get(), inputBrush);
	d2dDeviceContext->DrawGeometry(windowGeometry.Get(), borderBrush, 2.0f);
	d2dDeviceContext->DrawGeometry(inputGeometry.Get(), borderBrush, 2.0f);

	const auto position = GetWorldPosition();

	// Draw text
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 5.0f, position.y + 5.0f), textLayout.Get(), textBrush);

	// Draw Input Text
	if (inputActive)
	{
		std::wostringstream outInputValue;
		outInputValue << inputValue;
		if (inputActive)
		{
			if (hideCursor)
				outInputValue << "";
			else
				outInputValue << "|";
		}

		ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, 600.0f, 25.0f, inputValueTextLayout.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 4.0f, position.y + 224.0f), inputValueTextLayout.Get(), inputTextBrush);
	}
	else if (inputIndex == 0)
	{
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 4.0f, position.y + 224.0f), inputValueTextLayoutInactive.Get(), inputTextBrushInactive);
	}
}

const bool UITextWindow::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			inputActive = false;

			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible && !inputActive)
			{
				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y + 220.0f, position.x + 600.0f, position.y + 245.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					cursorTimer = 0.0f;
					hideCursor = false;
					inputActive = true;
				}
			}

			break;
		}
		case EventType::LeftMouseUp:
		{
			const auto mouseUpEvent = (MouseEvent*)event;



			break;
		}
		case EventType::KeyDown:
		{
			if (inputActive)
			{
				const auto keyDownEvent = (KeyDownEvent*)event;

				if (inputIndex <= 59)
				{
					cursorTimer = 0.0f;
					inputValue[inputIndex] = keyDownEvent->charCode;
					inputIndex++;
				}
			}

			break;
		}
		case EventType::SystemKeyDown:
		{
			if (inputActive)
			{
				const auto keyDownEvent = (SystemKeyDownEvent*)event;
				const auto keyCode = keyDownEvent->keyCode;

				switch (keyCode)
				{
					case VK_BACK:
					{
						if (inputIndex > 0)
						{
							cursorTimer = 0.0f;
							inputValue[inputIndex - 1] = 0;
							inputIndex--;
						}
						break;
					}
					case VK_RETURN:
					{
						if (inputIndex > 0)
						{
							cursorTimer = 0.0f;

							size_t i;
							char str[60];
							ZeroMemory(str, 60);
							wcstombs_s(&i, str, 60, inputValue, 60);

							// convert inputValue to const char * or std::string, then send the message
							g_eventHandler.QueueEvent(new SendChatMessage(new std::string(str)));

							while (inputIndex > 0)
							{
								inputValue[inputIndex - 1] = 0;
								inputIndex--;
							}
						}
					}
				}
			}

			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			inputActive = false;

			// Clear input text
			inputIndex = 0;
			ZeroMemory(inputValue, sizeof(inputValue));

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}

void UITextWindow::UpdateMessages()
{
	std::wostringstream text;

	for (auto i = 0; i < messages->size(); i++)
		text << messages->at(i)->c_str() << std::endl;

	ThrowIfFailed(writeFactory->CreateTextLayout(
		text.str().c_str(),
		(UINT32)text.str().size(),
		textFormat,
		590.0f,
		210.0f,
		textLayout.ReleaseAndGetAddressOf())
	);
}

void UITextWindow::Update(const float deltaTime)
{
	if (inputActive)
	{
		cursorTimer += deltaTime;

		if (cursorTimer >= 0.5f)
		{
			hideCursor = !hideCursor;
			cursorTimer = 0.0f;
		}
	}
}