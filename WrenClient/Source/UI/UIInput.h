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
	wchar_t inputValue[30] = { 0 };
	bool active{ false };
	bool secure{ false };
	float labelWidth{ 0.0f };
	float inputWidth{ 0.0f };
	float height{ 0.0f };
	std::string labelText;
	ID2D1SolidColorBrush* labelBrush{ nullptr };
    ID2D1SolidColorBrush* inputBrush{ nullptr };
    ID2D1SolidColorBrush* inputBorderBrush{ nullptr };
    ID2D1SolidColorBrush* inputValueBrush{ nullptr };
    IDWriteTextFormat* inputValueTextFormat{ nullptr };
	IDWriteTextFormat* labelTextFormat{ nullptr };
   
public:
	UIInput(
		UIComponentArgs uiComponentArgs,
		const bool secure,
		const float labelWidth,
		const float inputWidth,
		const float height,
		const char* labelText);
	void Initialize(
		ID2D1SolidColorBrush* labelBrush,
		ID2D1SolidColorBrush* inputBrush,
		ID2D1SolidColorBrush* inputBorderBrush,
		ID2D1SolidColorBrush* inputValueBrush,
		IDWriteTextFormat* inputValueTextFormat,
		IDWriteTextFormat* labelTextFormat);
    void Draw() override;
	const bool HandleEvent(const Event* const event) override;
    const wchar_t* GetInputValue() const;
	void ClearInput();
	const bool IsActive() const { return active; }
	void SetActive(const bool active) { this->active = active; }
};
