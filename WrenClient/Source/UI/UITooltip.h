#pragma once

#include "UIComponent.h"
#include "EventHandling/EventHandler.h"

class UITooltip : public UIComponent
{
	EventHandler& eventHandler;
	ID2D1SolidColorBrush* bodyBrush{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* textBrush{ nullptr };
	IDWriteTextFormat* textFormatTitle{ nullptr };
	IDWriteTextFormat* textFormatDescription{ nullptr };
	IDWriteFactory2* writeFactory{ nullptr };
	ID2D1DeviceContext1* d2dDeviceContext{ nullptr };
	ID2D1Factory2* d2dFactory{ nullptr };
	ComPtr<ID2D1RoundedRectangleGeometry> bodyGeometry;
	ComPtr<IDWriteTextLayout> headerTextLayout;
	ComPtr<IDWriteTextLayout> bodyTextLayout;
public:
	UITooltip(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		const std::string& headerText,
		const std::string& bodyText,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription,
		IDWriteFactory2* writeFactory,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};
