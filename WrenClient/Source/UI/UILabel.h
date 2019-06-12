#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UILabel : public UIComponent
{
	ComPtr<IDWriteTextLayout> textLayout;
	const float width{ 0.0f };
    char text[200];
    ID2D1SolidColorBrush* textBrush = nullptr;
    IDWriteTextFormat* textFormat = nullptr;
    IDWriteFactory2* writeFactory = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
public:
	UILabel(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		const float width,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormat,
		ID2D1DeviceContext1* d2dDeviceContext,
		IDWriteFactory2* writeFactory,
		ID2D1Factory2* d2dFactory);
	void Draw();
	virtual const bool HandleEvent(const Event* const event);
    void SetText(const char* arr);
};