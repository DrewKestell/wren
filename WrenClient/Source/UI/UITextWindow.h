#pragma once

#include <Constants.h>
#include <GameTimer.h>
#include "UIComponent.h"
#include <EventHandling/EventHandler.h>
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"
#include <ObjectManager.h>
#include "../Models/Item.h"

constexpr auto MESSAGES_PER_PAGE = 13;
constexpr auto TEXT_WINDOW_HEIGHT = 220.0f;
constexpr auto TEXT_WINDOW_WIDTH = 600.0f;
const std::wstring ENTER_MESSAGE{ L"Enter a message..." };

class UITextWindow : public UIComponent
{
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	std::vector<std::unique_ptr<Item>>& allItems;
	std::array<std::string, MESSAGE_BUFFER_SIZE> messages;
	unsigned int* messageIndex;
	ID2D1SolidColorBrush* backgroundBrush{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* inputBrush{ nullptr };
	ID2D1SolidColorBrush* inputTextBrush{ nullptr };
	ID2D1SolidColorBrush* inputTextBrushInactive{ nullptr };
	ID2D1SolidColorBrush* textBrush{ nullptr };
	ID2D1SolidColorBrush* scrollBarBackgroundBrush{ nullptr };
	ID2D1SolidColorBrush* scrollBarBrush{ nullptr };
	IDWriteTextFormat* textFormat{ nullptr };
	int playerId{ 0 };
	bool inputActive{ false };
	wchar_t inputValue[60];
	int inputIndex{ 0 };
	float cursorTimer{ 0.0f };
	bool hideCursor{ false };
	int minScrollIndex{ 0 };
	int scrollIndex{ 0 };
	int maxScrollIndex{ 0 };
	float scrollBarTopPosY{ 0.0f };
	float scrollBarBottomPosY{ 0.0f };
	ComPtr<ID2D1PathGeometry> pathGeometry;
	ComPtr<ID2D1GeometrySink> sink;
	ComPtr<IDWriteTextLayout> inputValueTextLayout;
	ComPtr<IDWriteTextLayout> inputValueTextLayoutInactive;
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RectangleGeometry> windowGeometry;
	ComPtr<ID2D1RectangleGeometry> inputGeometry;
	ComPtr<ID2D1RectangleGeometry> scrollBarBackgroundGeometry;
	ComPtr<ID2D1RectangleGeometry> scrollBarGeometry;

	void UpdateMessages();

public:
	UITextWindow(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		ObjectManager& objectManager,
		std::vector<std::unique_ptr<Item>>& allItems,
		std::array<std::string, MESSAGE_BUFFER_SIZE> textWindowMessages,
		unsigned int* messageIndex);
	void Initialize(
		ID2D1SolidColorBrush* backgroundBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* inputBrush,
		ID2D1SolidColorBrush* inputTextBrush,
		ID2D1SolidColorBrush* inputTextBrushInactive,
		ID2D1SolidColorBrush* textBrush,
		ID2D1SolidColorBrush* scrollBarBackgroundBrush,
		ID2D1SolidColorBrush* scrollBarBrush,
		IDWriteTextFormat* textFormat,
		IDWriteTextFormat* textFormatInactive);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void Update();
	void AddMessage(const std::string& message);
};
