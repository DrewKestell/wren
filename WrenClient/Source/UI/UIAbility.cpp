#include "stdafx.h"
#include <Utility.h>
#include "UIAbility.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/ActivateAbilityEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"
#include "Events/UIAbilityDroppedEvent.h"

extern EventHandler g_eventHandler;

UIAbility::UIAbility(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const int zIndex,
	const int abilityId,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	ID2D1SolidColorBrush* highlightBrush,
	ID2D1SolidColorBrush* abilityPressedBrush,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const float worldPosX,
	const float worldPosY,
	const float clientWidth,
	const float clientHeight,
	const XMMATRIX projectionTransform,
	const bool isDragging,
	const float mousePosX,
	const float mousePosY)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  abilityId{ abilityId },
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  clientWidth{ clientWidth },
	  clientHeight{ clientHeight },
	  d3dDevice{ d3dDevice },
	  vertexShader{ vertexShader },
	  pixelShader{ pixelShader },
	  texture{ texture },
	  vertexShaderBuffer{ vertexShaderBuffer },
	  vertexShaderSize{ vertexShaderSize },
	  d3dDeviceContext{ d3dDeviceContext },
	  highlightBrush{ highlightBrush },
	  abilityPressedBrush{ abilityPressedBrush },
	  projectionTransform{ projectionTransform },
	  isDragging{ isDragging },
	  lastDragX{ mousePosX },
	  lastDragY{ mousePosY }
{
}

void UIAbility::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();

	// create highlight
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(worldPos.x, worldPos.y, worldPos.x + HIGHLIGHT_WIDTH, worldPos.y + HIGHLIGHT_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());

	auto pos = XMFLOAT3{ worldPos.x + 18.0f, worldPos.y + 18.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	auto res = XMVector3Unproject(v, 0.0f, 0.0f, clientWidth, clientHeight, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, d3dDevice, vec.x, vec.y, SPRITE_WIDTH, SPRITE_WIDTH);

	if (isHovered && !isDragging)
	{
		const auto thickness = isPressed ? 5.0f : 3.0f;
		const auto brush = isPressed ? abilityPressedBrush : highlightBrush;
		d2dDeviceContext->DrawGeometry(highlightGeometry.Get(), brush, thickness);
	}

	if (abilityCopy)
		abilityCopy->Draw();
}

const bool UIAbility::HandleEvent(const Event* const event)
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

			if (isVisible)
			{
				const auto worldPos = GetWorldPosition();
				if (Utility::DetectClick(worldPos.x, worldPos.y, worldPos.x + 38.0f, worldPos.y + 38.0f, mousePosX, mousePosY))
					isHovered = true;
				else
					isHovered = false;

				// if the button is pressed, and the mouse starts moving, let's move the UIAbility
				if (isPressed)
				{
					if (!isDragging && !abilityCopy)
					{
						const auto dragBehavior = GetParent()->GetUIAbilityDragBehavior();

						if (dragBehavior == "COPY")
						{
							abilityCopy = new UIAbility(uiComponents, worldPos, uiLayer, 2, abilityId, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, vertexShader, pixelShader, texture, highlightBrush, abilityPressedBrush, vertexShaderBuffer, vertexShaderSize, worldPos.x, worldPos.y, clientWidth, clientHeight, projectionTransform, true, mousePosX, mousePosY);
							abilityCopy->SetVisible(true);
						}
						else if (dragBehavior == "MOVE")
						{
							isDragging = true;
							lastDragX = mousePosX;
							lastDragY = mousePosY;
						}

						g_eventHandler.QueueEvent(new StartDraggingUIAbilityEvent{ mousePosX, mousePosY, Utility::GetHotbarIndex(clientHeight, mousePosX, mousePosY) });
					}
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

			if (isVisible)
			{
				if (isHovered && !isDragging)
					isPressed = true;
			}

			break;
		}
		case EventType::LeftMouseUp:
		{
			const auto derivedEvent = (MouseEvent*)event;

			if (isDragging)
			{
				g_eventHandler.QueueEvent(new UIAbilityDroppedEvent{ this, derivedEvent->mousePosX, derivedEvent->mousePosY });
			}
			
			if (!isDragging && isVisible && isHovered && isPressed)
			{
				g_eventHandler.QueueEvent(new ActivateAbilityEvent{ abilityId });
			}

			isPressed = false;
			isDragging = false;

			break;
		}
		case EventType::UIAbilityDropped:
		{
			abilityCopy = nullptr;

			break;
		}
	}

	return false;
}

void UIAbility::DrawSprite()
{
	if (!isVisible) return;

	sprite->Draw(d3dDeviceContext, projectionTransform);
	if (abilityCopy)
		abilityCopy->DrawSprite();
}