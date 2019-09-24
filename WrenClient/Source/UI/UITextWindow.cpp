#include "stdafx.h"
#include "UITextWindow.h"
#include "Events/AttackMissEvent.h"
#include "Events/AttackHitEvent.h"
#include <EventHandling/Events/MouseEvent.h>
#include <EventHandling/Events/KeyDownEvent.h>
#include <EventHandling/Events/SystemKeyDownEvent.h>
#include <EventHandling/Events/ChangeActiveLayerEvent.h>
#include <EventHandling/Events/SendChatMessage.h>
#include <EventHandling/Events/EnterWorldSuccessEvent.h>

using namespace DX;

UITextWindow::UITextWindow(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	EventHandler& eventHandler,
	std::string* messages[MESSAGE_BUFFER_SIZE],
	unsigned int* messageIndex,
	ID2D1SolidColorBrush* backgroundBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* inputBrush,
	ID2D1SolidColorBrush* inputTextBrush,
	ID2D1SolidColorBrush* inputTextBrushInactive,
	ID2D1SolidColorBrush* textBrush,
	ID2D1SolidColorBrush* scrollBarBackgroundBrush,
	ID2D1SolidColorBrush* scrollBarBrush,
	ID2D1DeviceContext1* d2dDeviceContext,
	IDWriteFactory2* writeFactory,
	IDWriteTextFormat* textFormat,
	IDWriteTextFormat* textFormatInactive,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  eventHandler{ eventHandler },
	  messages{ messages },
	  messageIndex{ messageIndex },
	  backgroundBrush{ backgroundBrush },
	  borderBrush{ borderBrush },
	  inputBrush{ inputBrush },
	  inputTextBrush{ inputTextBrush },
	  inputTextBrushInactive{ inputTextBrushInactive },
	  textBrush{ textBrush },
	  scrollBarBackgroundBrush{ scrollBarBackgroundBrush },
	  scrollBarBrush{ scrollBarBrush },
	  d2dDeviceContext{ d2dDeviceContext },
	  writeFactory{ writeFactory },
	  textFormat{ textFormat },
	  textFormatInactive{ textFormatInactive },
	  d2dFactory{ d2dFactory }
{
	UpdateMessages();

	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + TEXT_WINDOW_WIDTH, position.y + TEXT_WINDOW_HEIGHT), windowGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y + TEXT_WINDOW_HEIGHT, position.x + TEXT_WINDOW_WIDTH, position.y + 245.0f), inputGeometry.ReleaseAndGetAddressOf());
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + TEXT_WINDOW_WIDTH - 20.0f, position.y, position.x + TEXT_WINDOW_WIDTH, position.y + TEXT_WINDOW_HEIGHT), scrollBarBackgroundGeometry.ReleaseAndGetAddressOf());

	std::wostringstream outInputValue;
	outInputValue << "Enter a message...";
	ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormatInactive, TEXT_WINDOW_WIDTH, 25.0f, inputValueTextLayoutInactive.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
}

void UITextWindow::Draw()
{
	if (!isVisible) return;

	// Draw window
	d2dDeviceContext->FillGeometry(windowGeometry.Get(), backgroundBrush);
	d2dDeviceContext->FillGeometry(inputGeometry.Get(), inputBrush);
	d2dDeviceContext->FillGeometry(scrollBarBackgroundGeometry.Get(), scrollBarBackgroundBrush);
	d2dDeviceContext->DrawGeometry(windowGeometry.Get(), borderBrush, 2.0f);
	d2dDeviceContext->DrawGeometry(inputGeometry.Get(), borderBrush, 2.0f);
	
	const auto position = GetWorldPosition();

	// Draw scrollbar
	auto maxScrollBarHeight = 180.0f;
	auto minScrollBarHeight = 20.0f;
	auto reductionSegmentSize = 160.0f / (MESSAGE_BUFFER_SIZE - MESSAGES_PER_PAGE);
	auto scrollBarHeight = 180.0f;
	auto index = *messageIndex;
	if (index > 13)
	{
		scrollBarHeight = std::max(20.0f, 180.0f - (reductionSegmentSize * (index - MESSAGES_PER_PAGE)));
	}
	auto yOffset = reductionSegmentSize * ((maxScrollIndex - scrollIndex + MESSAGE_BUFFER_SIZE) % MESSAGE_BUFFER_SIZE);
	scrollBarTopPosY = position.y + 20.0f + 180.0f - scrollBarHeight - yOffset;
	scrollBarBottomPosY = position.y + 20.0f + 180.0f - yOffset;
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + TEXT_WINDOW_WIDTH - 18.0f, scrollBarTopPosY, position.x + TEXT_WINDOW_WIDTH - 2.0f, scrollBarBottomPosY), scrollBarGeometry.ReleaseAndGetAddressOf());
	d2dDeviceContext->FillGeometry(scrollBarGeometry.Get(), scrollBarBrush);

	ThrowIfFailed(d2dFactory->CreatePathGeometry(pathGeometry.ReleaseAndGetAddressOf()));
	ThrowIfFailed(pathGeometry->Open(sink.ReleaseAndGetAddressOf()));

	// scrollbar up arrow
	auto upArrowStartingX = position.x + TEXT_WINDOW_WIDTH - 10.0f;
	auto upArrowStartingY = position.y + 6.66f;
	sink->BeginFigure(D2D1::Point2F(upArrowStartingX, upArrowStartingY), D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(D2D1::Point2F(upArrowStartingX + 4.0f, upArrowStartingY + 6.66f));
	sink->AddLine(D2D1::Point2F(upArrowStartingX - 4.0f, upArrowStartingY + 6.66f));
	sink->AddLine(D2D1::Point2F(upArrowStartingX, upArrowStartingY));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);

	// scrollbar down arrow
	auto downArrowStartingX = position.x + TEXT_WINDOW_WIDTH - 10.0f;
	auto downArrowStartingY = position.y + TEXT_WINDOW_HEIGHT - 6.66f;
	sink->BeginFigure(D2D1::Point2F(downArrowStartingX, downArrowStartingY), D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(D2D1::Point2F(downArrowStartingX + 4.0f, downArrowStartingY - 6.66f));
	sink->AddLine(D2D1::Point2F(downArrowStartingX - 4.0f, downArrowStartingY - 6.66f));
	sink->AddLine(D2D1::Point2F(downArrowStartingX, downArrowStartingY));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);

	sink->Close();
	d2dDeviceContext->FillGeometry(pathGeometry.Get(), scrollBarBrush);

	// Draw text
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 5.0f, position.y + 5.0f), textLayout.Get(), textBrush);

	// Draw Input Text
	std::wostringstream outInputValue;
	outInputValue << inputValue;
	if (inputActive)
	{
		if (hideCursor)
			outInputValue << "";
		else
			outInputValue << "|";
	}
	if (inputIndex > 0 || inputActive)
	{
		ThrowIfFailed(writeFactory->CreateTextLayout(outInputValue.str().c_str(), (UINT32)outInputValue.str().size(), textFormat, TEXT_WINDOW_WIDTH, 25.0f, inputValueTextLayout.ReleaseAndGetAddressOf())); // (height - 2) takes the border into account, and looks more natural
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 4.0f, position.y + 224.0f), inputValueTextLayout.Get(), inputTextBrush);
	}
	else
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 4.0f, position.y + 224.0f), inputValueTextLayoutInactive.Get(), inputTextBrushInactive);
}

const bool UITextWindow::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			inputActive = false;

			if (isVisible)
			{
				const auto mouseDownEvent = (MouseEvent*)event;

				const auto position = GetWorldPosition();
				if (Utility::DetectClick(position.x, position.y + TEXT_WINDOW_HEIGHT, position.x + TEXT_WINDOW_WIDTH, position.y + 245.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					cursorTimer = 0.0f;
					hideCursor = false;
					inputActive = true;
				}
				else if (Utility::DetectClick(position.x + TEXT_WINDOW_WIDTH - 20.0f, position.y, position.x + TEXT_WINDOW_WIDTH, position.y + 20.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					auto diff = (MESSAGE_BUFFER_SIZE + scrollIndex - minScrollIndex) % MESSAGE_BUFFER_SIZE;
					if (diff > 0)
					{
						scrollIndex = (MESSAGE_BUFFER_SIZE + (scrollIndex - 1)) % MESSAGE_BUFFER_SIZE;
						UpdateMessages();
					}
				}
				else if (Utility::DetectClick(position.x + TEXT_WINDOW_WIDTH - 20.0f, position.y + TEXT_WINDOW_HEIGHT - 20.0f, position.x + TEXT_WINDOW_WIDTH, position.y + TEXT_WINDOW_HEIGHT, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					auto diff = (MESSAGE_BUFFER_SIZE + maxScrollIndex - scrollIndex) % MESSAGE_BUFFER_SIZE;
					if (diff > 0)
					{
						scrollIndex = (scrollIndex + 1) % MESSAGE_BUFFER_SIZE;
						UpdateMessages();
					}
				}
				else if (Utility::DetectClick(position.x + TEXT_WINDOW_WIDTH - 20.0f, position.y + 20.0f, position.x + TEXT_WINDOW_WIDTH, scrollBarTopPosY, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					auto diff = (MESSAGE_BUFFER_SIZE + scrollIndex - minScrollIndex) % MESSAGE_BUFFER_SIZE;
					auto val = std::min(diff, MESSAGES_PER_PAGE);
					if (val > 0)
					{
						scrollIndex = (MESSAGE_BUFFER_SIZE + (scrollIndex - val)) % MESSAGE_BUFFER_SIZE;
						UpdateMessages();
					}
				}
				else if (Utility::DetectClick(position.x + TEXT_WINDOW_WIDTH - 20.0f, scrollBarBottomPosY, position.x + TEXT_WINDOW_WIDTH, position.y + TEXT_WINDOW_HEIGHT - 20.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
				{
					auto diff = (MESSAGE_BUFFER_SIZE + maxScrollIndex - scrollIndex) % MESSAGE_BUFFER_SIZE;
					auto val = std::min(diff, MESSAGES_PER_PAGE);
					if (val > 0)
					{
						scrollIndex = (scrollIndex + val) % MESSAGE_BUFFER_SIZE;
						UpdateMessages();
					}
				}

				// if anywhere in the text window was clicked, return true to stop propagation
				if (Utility::DetectClick(position.x, position.y, position.x + TEXT_WINDOW_WIDTH, position.y + TEXT_WINDOW_HEIGHT + 20.0f, mouseDownEvent->mousePosX, mouseDownEvent->mousePosY))
					return true;
			}   

			break;
		}
		case EventType::AttackHit:
		{
			const auto derivedEvent = (AttackHitEvent*)event;

			if (derivedEvent->attackerId == playerId)
			{
				AddMessage(new std::string("You swing and hit Dummy for " + std::to_string(derivedEvent->damage) + " points of damage!"));
			}
			else if (derivedEvent->targetId == playerId)
			{
				AddMessage(new std::string("Dummy swings and hits you for " + std::to_string(derivedEvent->damage) + " points of damage!"));
			}

			break;
		}
		case EventType::AttackMiss:
		{
			const auto derivedEvent = (AttackMissEvent*)event;

			if (derivedEvent->attackerId == playerId)
			{
				AddMessage(new std::string("You swing at Dummy and miss!"));
			}
			else if (derivedEvent->targetId == playerId)
			{
				AddMessage(new std::string("Dummy swings at you and misses!"));
			}

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
							std::unique_ptr<Event> e = std::make_unique<SendChatMessage>(std::string(str));
							eventHandler.QueueEvent(e);

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
		case EventType::EnterWorldSuccess:
		{
			const auto derivedEvent = (EnterWorldSuccessEvent*)event;

			playerId = derivedEvent->accountId;

			break;
		}
	}

	return false;
}

void UITextWindow::UpdateMessages()
{
	std::wostringstream text;

	auto index = *messageIndex;
	auto messagesToDisplay = std::min((int)index, MESSAGES_PER_PAGE);
	for (auto i = scrollIndex; i < scrollIndex + messagesToDisplay; i++)
		text << messages[i % MESSAGE_BUFFER_SIZE]->c_str() << std::endl;

	ThrowIfFailed(writeFactory->CreateTextLayout(
		text.str().c_str(),
		(UINT32)text.str().size(),
		textFormat,
		590.0f,
		210.0f,
		textLayout.ReleaseAndGetAddressOf())
	);
}

void UITextWindow::AddMessage(std::string* message)
{
	unsigned int& index = *messageIndex;
	auto normalizedIndex = index % MESSAGE_BUFFER_SIZE;
	messages[normalizedIndex] = message;
	index++;

	if (index >= MESSAGES_PER_PAGE)
	{
		auto tmp = normalizedIndex + 1 - MESSAGES_PER_PAGE;
		scrollIndex = (MESSAGE_BUFFER_SIZE + tmp) % MESSAGE_BUFFER_SIZE;
	}

	minScrollIndex = index < MESSAGE_BUFFER_SIZE ? 0 : (scrollIndex + MESSAGES_PER_PAGE) % MESSAGE_BUFFER_SIZE;
	maxScrollIndex = scrollIndex;

	UpdateMessages();
}

void UITextWindow::Update()
{
	if (inputActive)
	{
		cursorTimer += UPDATE_FREQUENCY;

		if (cursorTimer >= 0.5f)
		{
			hideCursor = !hideCursor;
			cursorTimer = 0.0f;
		}
	}
}