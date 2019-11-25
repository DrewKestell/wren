#pragma once


#include "UIComponent.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIInput : public UIComponent
{
	bool secure;
	float labelWidth;
	float inputWidth;
	float height;
	std::string labelText;
	ID2D1SolidColorBrush* labelBrush{ nullptr };
	ID2D1SolidColorBrush* inputBrush{ nullptr };
	ID2D1SolidColorBrush* inputBorderBrush{ nullptr };
	ID2D1SolidColorBrush* inputValueBrush{ nullptr };
	IDWriteTextFormat* inputValueTextFormat{ nullptr };
	IDWriteTextFormat* labelTextFormat{ nullptr };
	ComPtr<IDWriteTextLayout> labelTextLayout;
	ComPtr<IDWriteTextLayout> inputValueTextLayout;
	ComPtr<ID2D1RoundedRectangleGeometry> inputGeometry;
	int inputIndex{ 0 };
	wchar_t inputValue[30] = { 0 };
	bool active{ false };

	void CreateTextLayout();
	
	friend class UIInputGroup;
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
	void SetInputValue(const wchar_t* val, const int len); // TODO: fix me
	void ClearInput();
	const bool IsActive() const { return active; }
	void SetActive(const bool active) { this->active = active; }
};
