#pragma once

#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIInput : public UIComponent
{
	ComPtr<IDWriteTextLayout> labelTextLayout;
	ComPtr<IDWriteTextLayout> inputValueTextLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> inputGeometry;
	int inputIndex{ 0 };
    wchar_t inputValue[30];
	bool active{ false };
	bool secure{ false };
	float labelWidth{ 0.0f };
	float inputWidth{ 0.0f };
	float height{ 0.0f };
	ID2D1SolidColorBrush* labelBrush = nullptr;
    ID2D1SolidColorBrush* inputBrush = nullptr;
    ID2D1SolidColorBrush* inputBorderBrush = nullptr;
    ID2D1SolidColorBrush* inputValueBrush = nullptr;
    IDWriteTextFormat* inputValueTextFormat = nullptr;
    IDWriteFactory2* writeFactory = nullptr;  
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;
   
public:
	UIInput(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
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
		ID2D1Factory2* d2dFactory);
    virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
    const wchar_t* GetInputValue() const;
	const bool IsActive() const { return active; }
	void SetActive(const bool active) { this->active = active; }
};
