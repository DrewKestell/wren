#include "stdafx.h"
#include "UIAbility.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/ActivateAbilityEvent.h"
#include "EventHandling/Events/StartDraggingUIAbilityEvent.h"
#include "EventHandling/Events/ActivateAbilitySuccessEvent.h"
#include "Events/UIAbilityDroppedEvent.h"

using namespace DX;

extern float g_clientWidth;
extern float g_clientHeight;
extern XMMATRIX g_projectionTransform;
extern bool g_mouseIsDragging;

UIAbility::UIAbility(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	Ability* ability,
	const bool toggled,
	const bool isDragging,
	const float mousePosX,
	const float mousePosY)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  ability{ ability },
	  toggled{ toggled },
	  isDragging{ isDragging },
	  lastDragX{ mousePosX },
	  lastDragY{ mousePosY }
{
}

void UIAbility::Initialize(
	IDWriteTextFormat* headerTextFormat,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* headerBrush,
	ID2D1SolidColorBrush* highlightBrush,
	ID2D1SolidColorBrush* abilityPressedBrush,
	ID2D1SolidColorBrush* abilityToggledBrush,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	ID2D1SolidColorBrush* tooltipBodyBrush,
	ID2D1SolidColorBrush* tooltipBorderBrush,
	ID2D1SolidColorBrush* tooltipTextBrush,
	IDWriteTextFormat* tooltipTextFormatTitle,
	IDWriteTextFormat* tooltipTextFormatDescription)
{
	this->headerTextFormat = headerTextFormat;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->texture = texture;
	this->borderBrush = borderBrush;
	this->headerBrush = headerBrush;
	this->highlightBrush = highlightBrush;
	this->abilityPressedBrush = abilityPressedBrush;
	this->abilityToggledBrush = abilityToggledBrush;
	this->vertexShaderBuffer = vertexShaderBuffer;
	this->vertexShaderSize = vertexShaderSize;
	this->tooltipBodyBrush = tooltipBodyBrush;
	this->tooltipBorderBrush = tooltipBorderBrush;
	this->tooltipTextBrush = tooltipTextBrush;
	this->tooltipTextFormatTitle = tooltipTextFormatTitle;
	this->tooltipTextFormatDescription = tooltipTextFormatDescription;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(ability->name).c_str(),
			static_cast<unsigned int>(ability->name.size()),
			headerTextFormat,
			ABILITY_HEADER_WIDTH,
			ABILITY_HEADER_HEIGHT,
			headerTextLayout.ReleaseAndGetAddressOf()
		)
	);

	tooltip = std::make_unique<UITooltip>(UIComponentArgs{ deviceResources, uiComponents, [](const float, const float) { return XMFLOAT2{ 0.0f, 64.0f }; }, uiLayer, 9999 }, eventHandler, ability->name, ability->description);
	AddChildComponent(*tooltip.get());
	tooltip->Initialize(tooltipBodyBrush, tooltipBorderBrush, tooltipTextBrush, tooltipTextFormatTitle, tooltipTextFormatDescription);
}

// we have different Draw methods here because UIAbility is used in both
// the UIAbilitiesContainer and the UIHotbar, and the leader and border should
// only be drawn in the former. we should probably refactor this.
void UIAbility::DrawHeadersAndBorder()
{
	if (!isDragging)
	{
		const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
		const auto worldPos = GetWorldPosition();

		// draw header
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(worldPos.x, worldPos.y), headerTextLayout.Get(), headerBrush);
	
		// draw border
		d2dDeviceContext->DrawGeometry(borderGeometry.Get(), borderBrush, 2.0f);
	}
}

void UIAbility::DrawHoverAndToggledBorders()
{
	if (!isDragging)
	{
		if (isHovered && !g_mouseIsDragging)
		{
			const auto thickness = isPressed ? 5.0f : 3.0f;
			const auto brush = isPressed ? abilityPressedBrush : highlightBrush;
			deviceResources->GetD2DDeviceContext()->DrawGeometry(highlightGeometry.Get(), brush, thickness);
		}

		// draw toggled border
		if (isToggled)
			deviceResources->GetD2DDeviceContext()->DrawGeometry(highlightGeometry.Get(), abilityToggledBrush, 4.0f);
	}
}

void UIAbility::Draw()
{
	if (!isVisible) return;

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	
	d2dDeviceContext->EndDraw();

	sprite->Draw(deviceResources->GetD3DDeviceContext());

	d2dDeviceContext->BeginDraw();

	if (abilityCopy)
		abilityCopy->Draw();
}

const bool UIAbility::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

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
			if (Utility::DetectClick(worldPos.x, worldPos.y + 20.0f, worldPos.x + 38.0f, worldPos.y + 58.0f, mousePosX, mousePosY))
				isHovered = true;
			else
				isHovered = false;

			// if the button is pressed, and the mouse starts moving, let's move the UIAbility
			if (isPressed && !isDragging && !abilityCopy)
			{
				const auto dragBehavior = GetParent()->GetUIAbilityDragBehavior();

				if (dragBehavior == "COPY")
				{
					abilityCopy = new UIAbility(UIComponentArgs{ deviceResources, uiComponents, calculatePosition, uiLayer, zIndex + 1 }, eventHandler, ability, toggled, true, mousePosX, mousePosY);
					abilityCopy->Initialize(headerTextFormat, vertexShader, pixelShader, texture, borderBrush, headerBrush, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize, tooltipBodyBrush, tooltipBorderBrush, tooltipTextBrush, tooltipTextFormatTitle, tooltipTextFormatDescription);
					abilityCopy->SetLocalPosition(XMFLOAT2{ mousePosX - (ABILITY_SPRITE_WIDTH / 2), mousePosY - ABILITY_SPRITE_WIDTH });
					abilityCopy->isVisible = true;
					abilityCopy->isToggled = isToggled;
					abilityCopy->CreatePositionDependentResources();
				}
				else if (dragBehavior == "MOVE")
				{
					isDragging = true;
					lastDragX = mousePosX;
					lastDragY = mousePosY;
				}

				std::unique_ptr<Event> e = std::make_unique<StartDraggingUIAbilityEvent>(mousePosX, mousePosY, Utility::GetHotbarIndex(g_clientHeight, mousePosX, mousePosY));
				eventHandler.QueueEvent(e);
			}

			if (isDragging)
			{
				const auto deltaX = mousePosX - lastDragX;
				const auto deltaY = mousePosY - lastDragY;

				Translate(XMFLOAT2(deltaX, deltaY));

				lastDragX = mousePosX;
				lastDragY = mousePosY;

				CreatePositionDependentResources();
			}

			if (isHovered && !g_mouseIsDragging)
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

			auto stopPropagation = false;

			if (isDragging)
			{
				std::unique_ptr<Event> e = std::make_unique<UIAbilityDroppedEvent>(this, derivedEvent->mousePosX, derivedEvent->mousePosY);
				eventHandler.QueueEvent(e);
			}
			
			if (!isDragging && isVisible && isHovered && isPressed)
			{
				std::unique_ptr<Event> e = std::make_unique<ActivateAbilityEvent>(ability->abilityId);
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

			if (ability->abilityId == derivedEvent->abilityId)
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
				std::unique_ptr<Event> e = std::make_unique<ActivateAbilityEvent>(ability->abilityId);
				eventHandler.QueueEvent(e);
			}
		}
		case EventType::WindowResize:
		{
			CreatePositionDependentResources();

			break;
		}
	}

	return false;
}

void UIAbility::CreatePositionDependentResources()
{
	const auto d2dFactory = deviceResources->GetD2DFactory();
	const auto worldPos = GetWorldPosition();
	auto positionX = worldPos.x;
	auto positionY = worldPos.y + 20.0f;

	// create border
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(positionX, positionY, positionX + ABILITY_BORDER_WIDTH, positionY + ABILITY_BORDER_WIDTH), borderGeometry.ReleaseAndGetAddressOf());

	// create highlight
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(positionX, positionY, positionX + HIGHLIGHT_WIDTH, positionY + HIGHLIGHT_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());

	// create toggle geometry
	if (toggled)
		d2dFactory->CreateRectangleGeometry(D2D1::RectF(positionX, positionY, positionX + HIGHLIGHT_WIDTH, positionY + HIGHLIGHT_WIDTH), toggledGeometry.ReleaseAndGetAddressOf());

	// create sprite
	XMFLOAT3 pos{ positionX + 20.0f, positionY + 20.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	auto res = XMVector3Unproject(v, 0.0f, 0.0f, g_clientWidth, g_clientHeight, 0.0f, 1000.0f, g_projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, deviceResources->GetD3DDevice(), vec.x, vec.y, ABILITY_SPRITE_WIDTH, ABILITY_SPRITE_WIDTH, zIndex);

	tooltip->CreatePositionDependentResources();
}

// TODO: tooltips are normally below the hover. this is a hack to fix uiabilities you drop on the hotbar
// at the bottom of the client. refactor this to be smarter about where the tooltip is drawn.
// you can probably check the pos of the mouse relative to the client window bounds when
// the tooltip is hovered.
void UIAbility::SetTooltipPositionAbove()
{
	tooltip->SetLocalPosition(XMFLOAT2{ 0.0f, localPosition.y - tooltip->GetHeight() + 36.0f });
	tooltip->CreatePositionDependentResources();
}
