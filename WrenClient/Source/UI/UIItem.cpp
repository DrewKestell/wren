#include "stdafx.h"
#include "UIItem.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "Events/UIItemDroppedEvent.h"
#include "UILootContainer.h"

UIItem::UIItem(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	ClientSocketManager& socketManager,
	const int itemId,
	const std::string& name,
	const std::string& description,
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
	ID2D1SolidColorBrush* bodyBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormatTitle,
	IDWriteTextFormat* textFormatDescription,
	const bool isDragging,
	const float mousePosX,
	const float mousePosY)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  socketManager{ socketManager },
	  itemId{ itemId },
	  name{ name },
	  description{ description },
	  clientWidth{ clientWidth },
	  clientHeight{ clientHeight },
	  vertexShader{ vertexShader },
	  pixelShader{ pixelShader },
	  texture{ texture },
	  grayTexture{ grayTexture },
	  vertexShaderBuffer{ vertexShaderBuffer },
	  vertexShaderSize{ vertexShaderSize },
	  highlightBrush{ highlightBrush },
	  projectionTransform{ projectionTransform },
	  bodyBrush{ bodyBrush },
	  borderBrush{ borderBrush },
	  textBrush{ textBrush },
	  textFormatTitle{ textFormatTitle },
	  textFormatDescription{ textFormatDescription },
	  isDragging{ isDragging },
	  lastDragX{ mousePosX },
	  lastDragY{ mousePosY }
{
	tooltip = std::make_unique<UITooltip>(UIComponentArgs{ deviceResources, uiComponents, XMFLOAT2{ -3.0f, 42.0f }, uiLayer, zIndex + 1 }, eventHandler, name, description, bodyBrush, borderBrush, textBrush, textFormatTitle, textFormatDescription);
	AddChildComponent(*tooltip.get());
}

void UIItem::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();

	// create highlight
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(worldPos.x, worldPos.y, worldPos.x + SPRITE_SIZE, worldPos.y + SPRITE_SIZE), highlightGeometry.ReleaseAndGetAddressOf());

	XMFLOAT3 pos{ worldPos.x + 18.0f, worldPos.y + 18.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	auto res = XMVector3Unproject(v, 0.0f, 0.0f, clientWidth, clientHeight, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, deviceResources->GetD3DDevice(), vec.x, vec.y, SPRITE_SIZE, SPRITE_SIZE);

	if (isHovered && !isDragging)
	{
		deviceResources->GetD2DDeviceContext()->DrawGeometry(highlightGeometry.Get(), highlightBrush, 3.0f);
	}

	deviceResources->GetD2DDeviceContext()->EndDraw();

	sprite->Draw(deviceResources->GetD3DDeviceContext(), projectionTransform);

	deviceResources->GetD2DDeviceContext()->BeginDraw();
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
			if (!isVisible)
				return false;

			const auto derivedEvent = (MouseEvent*)event;
			const auto mousePosX = derivedEvent->mousePosX;
			const auto mousePosY = derivedEvent->mousePosY;

			const auto worldPos = GetWorldPosition();
			if (Utility::DetectClick(worldPos.x, worldPos.y, worldPos.x + 38.0f, worldPos.y + 38.0f, mousePosX, mousePosY))
				isHovered = true;
			else
				isHovered = false;

			if (GetParent()->GetUIItemDragBehavior() == "MOVE")
			{
				// if the button is pressed, and the mouse starts moving, let's move the UIItem
				if (isPressed && !isDragging && !itemCopy)
				{
					itemCopy = new UIItem(UIComponentArgs{ deviceResources, uiComponents, worldPos, uiLayer, 2 }, eventHandler, socketManager, itemId, name, description, vertexShader, pixelShader, texture, grayTexture, highlightBrush, vertexShaderBuffer, vertexShaderSize, clientWidth, clientHeight, projectionTransform, bodyBrush, borderBrush, textBrush, textFormatTitle, textFormatDescription, true, mousePosX, mousePosY);
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

			if (isHovered)
				tooltip->isVisible = true;
			else
				tooltip->isVisible = false;

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
