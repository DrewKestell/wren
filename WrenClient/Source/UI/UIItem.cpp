#include "stdafx.h"
#include "UIItem.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/MouseEvent.h"
#include "Events/UIItemDroppedEvent.h"
#include "UILootContainer.h"

UIItem::UIItem(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const unsigned int zIndex,
	EventHandler& eventHandler,
	ClientSocketManager& socketManager,
	const int itemId,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	ID3D11ShaderResourceView* grayTexture,
	ID2D1SolidColorBrush* highlightBrush,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const float clientWidth,
	const float clientHeight,
	const XMMATRIX projectionTransform,
	const bool isDragging,
	const float mousePosX,
	const float mousePosY)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  eventHandler{ eventHandler },
	  socketManager{ socketManager },
	  itemId{ itemId },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  clientWidth{ clientWidth },
	  clientHeight{ clientHeight },
	  d3dDevice{ d3dDevice },
	  vertexShader{ vertexShader },
	  pixelShader{ pixelShader },
	  texture{ texture },
	  grayTexture{ grayTexture },
	  vertexShaderBuffer{ vertexShaderBuffer },
	  vertexShaderSize{ vertexShaderSize },
	  d3dDeviceContext{ d3dDeviceContext },
	  highlightBrush{ highlightBrush },
	  projectionTransform{ projectionTransform },
	  isDragging{ isDragging },
	  lastDragX{ mousePosX },
	  lastDragY{ mousePosY }
{
}

void UIItem::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();

	// create highlight
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(worldPos.x, worldPos.y, worldPos.x + SPRITE_SIZE, worldPos.y + SPRITE_SIZE), highlightGeometry.ReleaseAndGetAddressOf());

	XMFLOAT3 pos{ worldPos.x + 18.0f, worldPos.y + 18.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	auto res = XMVector3Unproject(v, 0.0f, 0.0f, clientWidth, clientHeight, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, d3dDevice, vec.x, vec.y, SPRITE_SIZE, SPRITE_SIZE);

	if (isHovered && !isDragging)
	{
		d2dDeviceContext->DrawGeometry(highlightGeometry.Get(), highlightBrush, 3.0f);
	}

	d2dDeviceContext->EndDraw();

	sprite->Draw(d3dDeviceContext, projectionTransform);

	d2dDeviceContext->BeginDraw();
}

const bool UIItem::HandleEvent(const Event* const event)
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
		case EventType::MouseMove:
		{
			const auto derivedEvent = (MouseEvent*)event;
			const auto mousePosX = derivedEvent->mousePosX;
			const auto mousePosY = derivedEvent->mousePosY;

			if (isVisible && GetParent()->GetUIItemDragBehavior() == "MOVE")
			{
				const auto worldPos = GetWorldPosition();
				if (Utility::DetectClick(worldPos.x, worldPos.y, worldPos.x + 38.0f, worldPos.y + 38.0f, mousePosX, mousePosY))
					isHovered = true;
				else
					isHovered = false;

				// if the button is pressed, and the mouse starts moving, let's move the UIItem
				if (isPressed && !isDragging && !itemCopy)
				{
					itemCopy = new UIItem(uiComponents, worldPos, uiLayer, 2, eventHandler, socketManager, itemId, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, vertexShader, pixelShader, texture, grayTexture, highlightBrush, vertexShaderBuffer, vertexShaderSize, clientWidth, clientHeight, projectionTransform, true, mousePosX, mousePosY);
					itemCopy->isVisible = true;

					/*std::unique_ptr<Event> e = std::make_unique<StartDraggingUIAbilityEvent>(mousePosX, mousePosY, Utility::GetHotbarIndex(clientHeight, mousePosX, mousePosY));
					eventHandler.QueueEvent(e);*/
				}

				if (isDragging)
				{
					const auto deltaX = mousePosX - lastDragX;
					const auto deltaY = mousePosY - lastDragY;

					Translate(XMFLOAT2(deltaX, deltaY));

					lastDragX = mousePosX;
					lastDragY = mousePosY;
				}
			}

			break;
		}
		case EventType::LeftMouseDown:
		{
			const auto derivedEvent = (MouseEvent*)event;

			if (isVisible && isHovered && !isDragging)
			{
				isPressed = true;
				return true;
			}

			break;
		}
		case EventType::LeftMouseUp:
		{
			const auto derivedEvent = (MouseEvent*)event;

			if (isDragging)
			{
				std::unique_ptr<Event> e = std::make_unique<UIItemDroppedEvent>(this, derivedEvent->mousePosX, derivedEvent->mousePosY);
				eventHandler.QueueEvent(e);
			}

			isPressed = false;
			isDragging = false;

			break;
		}
		case EventType::UIAbilityDropped:
		{
			itemCopy = nullptr;

			break;
		}
		case EventType::RightMouseDown:
		{
			const auto derivedEvent = (MouseEvent*)event;
			const auto mousePosX = derivedEvent->mousePosX;
			const auto mousePosY = derivedEvent->mousePosY;
			const auto worldPos = GetWorldPosition();

			if (Utility::DetectClick(worldPos.x, worldPos.y, worldPos.x + 38.0f, worldPos.y + 38.0f, mousePosX, mousePosY))
			{
				const auto parent = GetParent();
				if (isVisible && parent->GetUIItemRightClickBehavior() == "LOOT")
				{
					const auto uiLootContainer = (UILootContainer*)parent;
					for (auto i = 0; i < uiLootContainer->uiItems.size(); i++)
					{
						if (uiLootContainer->uiItems[i].get() == this)
						{
							std::vector<std::string> args{ std::to_string(uiLootContainer->currentGameObjectId), std::to_string(i) };
							socketManager.SendPacket(OpCode::LootItem, args);
						}
					}
				}

				return true;
			}
		}
	}

	return false;
}
