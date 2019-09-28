#include "stdafx.h"
#include "UILootContainer.h"
#include "UIPanel.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UILootContainer::UILootContainer(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	ID2D1SolidColorBrush* brush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  brush{ brush },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory }
{
}

void UILootContainer::Draw()
{
	if (!isVisible) return;

	// FIXME: this should be optimized. see comment below in HandleEvent.
	ReinitializeGeometry();

	for (auto i = 0; i < 12; i++)
	{
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);
	}
}

const bool UILootContainer::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
		// this is a decent idea to optimize, but it's not working correctly.
		// this component gets the MouseMove event BEFORE the UIPanel.
		// so it reinitializes its geometry based on the UIPanel's old position
		// then the UIPanel gets the MouseMove event and moves its position,
		// making the boxes inside the panel seem to jiggle around.
		/*case EventType::MouseMove:
		{
			const auto parent = static_cast<UIPanel*>(GetParent());
			
			if (parent->GetIsDragging())
				ReinitializeGeometry();

			break;
		}*/
	}

	return false;
}

void UILootContainer::ReinitializeGeometry()
{
	const auto position = GetWorldPosition();
	const auto width = 40.0f;
	auto xOffset = 5.0f;
	auto yOffset = 25.0f;
	for (auto i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 3; j++)
		{
			d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + xOffset, position.y + yOffset, position.x + xOffset + width, position.y + yOffset + width), geometry[j + (3 * i)].ReleaseAndGetAddressOf());
			xOffset += 45.0f;
		}

		yOffset += 45.0f;
		xOffset = 5.0f;
	}
}