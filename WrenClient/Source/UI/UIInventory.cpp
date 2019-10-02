#include "stdafx.h"
#include "UIInventory.h"
#include "UIPanel.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UIInventory::UIInventory(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	EventHandler& eventHandler,
	std::vector<std::unique_ptr<Item>>& allItems,
	std::vector<ComPtr<ID3D11ShaderResourceView>> allTextures,
	ID2D1SolidColorBrush* brush,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	ID2D1SolidColorBrush* highlightBrush,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const XMMATRIX projectionTransform,
	const float clientWidth,
	const float clientHeight)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	eventHandler{ eventHandler },
	allItems{ allItems },
	allTextures{ allTextures },
	brush{ brush },
	d2dDeviceContext{ d2dDeviceContext },
	d2dFactory{ d2dFactory },
	d3dDevice{ d3dDevice },
	d3dDeviceContext{ d3dDeviceContext },
	highlightBrush{ highlightBrush },
	vertexShader{ vertexShader },
	pixelShader{ pixelShader },
	vertexShaderBuffer{ vertexShaderBuffer },
	vertexShaderSize{ vertexShaderSize },
	projectionTransform{ projectionTransform },
	clientWidth{ clientWidth },
	clientHeight{ clientHeight }
{
}

void UIInventory::Draw()
{
	if (!isVisible) return;

	// FIXME: this should be optimized. see comment below in HandleEvent.
	ReinitializeGeometry();

	for (auto i = 0; i < INVENTORY_SIZE; i++)
	{
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);
	}
}

void UIInventory::DrawSprites()
{
	for (auto i = 0; i < uiItems.size(); i++)
	{
		const auto uiItem = uiItems.at(i).get();
		if (uiItem)
			uiItem->DrawSprite();
	}
}

const bool UIInventory::HandleEvent(const Event* const event)
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

void UIInventory::ReinitializeGeometry()
{
	const auto position = GetWorldPosition();
	const auto width = 40.0f;
	auto xOffset = 5.0f;
	auto yOffset = 25.0f;
	for (auto i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 4; j++)
		{
			d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x + xOffset, position.y + yOffset, position.x + xOffset + width, position.y + yOffset + width), geometry[j + (4 * i)].ReleaseAndGetAddressOf());
			xOffset += 45.0f;
		}

		yOffset += 45.0f;
		xOffset = 5.0f;
	}
}

const std::string UIInventory::GetUIItemDragBehavior() const
{
	return "MOVE";
}