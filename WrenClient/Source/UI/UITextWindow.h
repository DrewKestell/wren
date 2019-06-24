#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UITextWindow : public UIComponent
{
	std::vector<std::string*>* messages;
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1RectangleGeometry> windowGeometry;
	ID2D1SolidColorBrush* backgroundBrush;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* textBrush;
	ID2D1DeviceContext1* d2dDeviceContext;
	IDWriteFactory2* writeFactory;
	IDWriteTextFormat* textFormat;
	ID2D1Factory2* d2dFactory;

public:
	UITextWindow(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		std::vector<std::string*>* messages,
		ID2D1SolidColorBrush* backgroundBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		IDWriteTextFormat* textFormat,
		ID2D1Factory2* d2dFactory);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void UpdateMessages();
};
