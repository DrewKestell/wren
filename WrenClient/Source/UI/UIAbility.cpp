#include "stdafx.h"
#include "UIAbility.h"
#include "../Events/WindowResizeEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/ActivateAbilityEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"
#include "EventHandling/Events/ActivateAbilitySuccessEvent.h"
#include "Events/UIAbilityDroppedEvent.h"

UIAbility::UIAbility(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	const int abilityId,
	const bool toggled,
	const float clientWidth,
	const float clientHeight,
	const bool isDragging,
	const float mousePosX,
	const float mousePosY)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  abilityId{ abilityId },
	  toggled{ toggled },
	  clientWidth{ clientWidth },
	  clientHeight{ clientHeight },
	  isDragging{ isDragging },
	  lastDragX{ mousePosX },
	  lastDragY{ mousePosY }
{
}

void UIAbility::Initialize(
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	ID2D1SolidColorBrush* highlightBrush,
	ID2D1SolidColorBrush* abilityPressedBrush,
	ID2D1SolidColorBrush* abilityToggledBrush,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const XMMATRIX projectionTransform
)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->texture = texture;
	this->highlightBrush = highlightBrush;
	this->abilityPressedBrush = abilityPressedBrush;
	this->abilityToggledBrush = abilityToggledBrush;
	this->vertexShaderBuffer = vertexShaderBuffer;
	this->vertexShaderSize = vertexShaderSize;
	this->projectionTransform = projectionTransform;
}

void UIAbility::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();

	// create highlight
	deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(worldPos.x, worldPos.y, worldPos.x + HIGHLIGHT_WIDTH, worldPos.y + HIGHLIGHT_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());

	// create toggle geometry
	if (toggled)
		deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(worldPos.x, worldPos.y, worldPos.x + HIGHLIGHT_WIDTH, worldPos.y + HIGHLIGHT_WIDTH), toggledGeometry.ReleaseAndGetAddressOf());

	XMFLOAT3 pos{ worldPos.x + 18.0f, worldPos.y + 18.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	auto res = XMVector3Unproject(v, 0.0f, 0.0f, clientWidth, clientHeight, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, deviceResources->GetD3DDevice(), vec.x, vec.y, SPRITE_WIDTH, SPRITE_WIDTH);

	if (isHovered && !isDragging)
	{
		const auto thickness = isPressed ? 5.0f : 3.0f;
		const auto brush = isPressed ? abilityPressedBrush : highlightBrush;
		deviceResources->GetD2DDeviceContext()->DrawGeometry(highlightGeometry.Get(), brush, thickness);
	}
	
	if (isToggled)
		deviceResources->GetD2DDeviceContext()->DrawGeometry(highlightGeometry.Get(), abilityToggledBrush, 4.0f);

	deviceResources->GetD2DDeviceContext()->EndDraw();

	sprite->Draw(deviceResources->GetD3DDeviceContext(), projectionTransform);

	deviceResources->GetD2DDeviceContext()->BeginDraw();

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
				if (isPressed && !isDragging && !abilityCopy)
				{
					const auto dragBehavior = GetParent()->GetUIAbilityDragBehavior();

					if (dragBehavior == "COPY")
					{
						abilityCopy = new UIAbility(UIComponentArgs{ deviceResources, uiComponents, calculatePosition, uiLayer, 2 }, eventHandler, abilityId, toggled, vertexShader, pixelShader, texture, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize, clientWidth, clientHeight, projectionTransform, true, mousePosX, mousePosY);
						abilityCopy->isVisible = true;
						abilityCopy->isToggled = isToggled;
					}
					else if (dragBehavior == "MOVE")
					{
						isDragging = true;
						lastDragX = mousePosX;
						lastDragY = mousePosY;
					}

					std::unique_ptr<Event> e = std::make_unique<StartDraggingUIAbilityEvent>(mousePosX, mousePosY, Utility::GetHotbarIndex(clientHeight, mousePosX, mousePosY));
					eventHandler.QueueEvent(e);
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

			auto stopPropagation = false;

			if (isDragging)
			{
				std::unique_ptr<Event> e = std::make_unique<UIAbilityDroppedEvent>(this, derivedEvent->mousePosX, derivedEvent->mousePosY);
				eventHandler.QueueEvent(e);
			}
			
			if (!isDragging && isVisible && isHovered && isPressed)
			{
				std::unique_ptr<Event> e = std::make_unique<ActivateAbilityEvent>(abilityId);
				eventHandler.QueueEvent(e);
				stopPropagation = true;
			}

			isPressed = false;
			isDragging = false;

			return stopPropagation;
		}
		case EventType::UIAbilityDropped:
		{
			abilityCopy = nullptr;

			break;
		}
		case EventType::ActivateAbilitySuccess:
		{
			const auto derivedEvent = (ActivateAbilitySuccessEvent*)event;

			if (abilityId == derivedEvent->abilityId)
			{
				if (toggled)
					isToggled = !isToggled;
			}

			break;
		}
		case EventType::UnsetTarget:
		{
			// if any abilities require a target, toggle them off

			if (isToggled)
			{
				std::unique_ptr<Event> e = std::make_unique<ActivateAbilityEvent>(abilityId);
				eventHandler.QueueEvent(e);
			}
		}
		case EventType::WindowResize:
		{
			const auto derivedEvent = (WindowResizeEvent*)event;

			clientWidth = derivedEvent->width;
			clientHeight = derivedEvent->height;

			break;
		}
	}

	return false;
}
