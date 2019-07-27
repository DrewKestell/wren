#pragma once

#include <Constants.h>
#include <GameTimer.h>
#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

static constexpr int MESSAGES_PER_PAGE = 13;
static constexpr float TEXT_WINDOW_HEIGHT = 220.0f;
static constexpr float TEXT_WINDOW_WIDTH = 600.0f;

class UITextWindow : public UIComponent
{
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
	std::string** messages;
	unsigned int* messageIndex;
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RectangleGeometry> windowGeometry;
	ComPtr<ID2D1RectangleGeometry> inputGeometry;
	ComPtr<ID2D1RectangleGeometry> scrollBarBackgroundGeometry;
	ComPtr<ID2D1RectangleGeometry> scrollBarGeometry;
	ID2D1SolidColorBrush* backgroundBrush;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* inputBrush;
	ID2D1SolidColorBrush* inputTextBrush;
	ID2D1SolidColorBrush* inputTextBrushInactive;
	ID2D1SolidColorBrush* textBrush;
	ID2D1SolidColorBrush* scrollBarBackgroundBrush;
	ID2D1SolidColorBrush* scrollBarBrush;
	ID2D1DeviceContext1* d2dDeviceContext;
	IDWriteFactory2* writeFactory;
	IDWriteTextFormat* textFormat;
	IDWriteTextFormat* textFormatInactive;
	ID2D1Factory2* d2dFactory;

	void UpdateMessages();

public:
	UITextWindow(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
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
		ID2D1Factory2* d2dFactory);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void Update(const float deltaTime);
	void AddMessage(std::string* message);
};
