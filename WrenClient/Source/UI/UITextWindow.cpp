#include "stdafx.h"
#include "Layer.h"
#include "UITextWindow.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UITextWindow::UITextWindow(
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
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	messages{ messages },
	backgroundBrush{ backgroundBrush },
	borderBrush{ borderBrush },
	textBrush{ textBrush },
	d2dDeviceContext{ d2dDeviceContext },
	writeFactory{ writeFactory },
	textFormat{ textFormat },
	d2dFactory{ d2dFactory }
{
	UpdateMessages();

	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + 600.0f, position.y + 245.0f), windowGeometry.ReleaseAndGetAddressOf());
}

void UITextWindow::Draw()
{
	if (!isVisible) return;

	// Draw window
	d2dDeviceContext->FillGeometry(windowGeometry.Get(), backgroundBrush);
	d2dDeviceContext->DrawGeometry(windowGeometry.Get(), borderBrush, 2.0f);

	const auto position = GetWorldPosition();

	// Draw text
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(position.x + 5.0f, position.y + 5.0f), textLayout.Get(), textBrush);
}

const bool UITextWindow::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::LeftMouseDown:
		{
			const auto mouseDownEvent = (MouseEvent*)event;

			if (isVisible)
			{

			}

			break;
		}
		case EventType::LeftMouseUp:
		{
			const auto mouseUpEvent = (MouseEvent*)event;



			break;
		}
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

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
		235.0f,
		textLayout.ReleaseAndGetAddressOf())
	);
}