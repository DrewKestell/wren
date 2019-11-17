#include "stdafx.h"
#include "UIInventory.h"
#include "Events/UIItemDroppedEvent.h"
#include "Events/MoveItemSuccessEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/LootItemSuccessEvent.h"
#include "EventHandling/Events/StartDraggingUIItemEvent.h"

UIInventory::UIInventory(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	ClientSocketManager& socketManager,
	std::vector<std::unique_ptr<Item>>& allItems,
	std::vector<ComPtr<ID3D11ShaderResourceView>>& allTextures)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  socketManager{ socketManager },
	  allItems{ allItems },
	  allTextures{ allTextures }
{
}

void UIInventory::Initialize(
	ID2D1SolidColorBrush* brush,
	ID2D1SolidColorBrush* highlightBrush,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	ID2D1SolidColorBrush* bodyBrush,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* textBrush,
	IDWriteTextFormat* textFormatTitle,
	IDWriteTextFormat* textFormatDescription
)
{
	this->brush = brush;
	this->highlightBrush = highlightBrush;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->vertexShaderBuffer = vertexShaderBuffer;
	this->vertexShaderSize = vertexShaderSize;
	this->bodyBrush = bodyBrush;
	this->borderBrush = borderBrush;
	this->textBrush = textBrush;
	this->textFormatTitle = textFormatTitle;
	this->textFormatDescription = textFormatDescription;
}

void UIInventory::Draw()
{
	if (!isVisible) return;

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();

	for (auto i = 0; i < INVENTORY_SIZE; i++)
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);
}

const bool UIInventory::HandleEvent(const Event* const event)
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
		case EventType::LootItemSuccess:
		{
			const auto derivedEvent = (LootItemSuccessEvent*)event;

			if (playerId == derivedEvent->looterId)
			{
				const auto destinationSlot = derivedEvent->destinationSlot;

				const auto item = allItems.at(derivedEvent->itemId - 1).get();
				items.at(destinationSlot) = item;

				// TODO: refactor this into a function and call it here and below
				const auto row = destinationSlot / 4;
				const auto col = destinationSlot % 4;
				const auto posX = 5.0f + (col * 45.0f);
				const auto posY = 25.0f + (row * 45.0f);
				const auto texture = allTextures.at(item->GetSpriteId()).Get();
				const auto grayTexture = allTextures.at(item->GetGraySpriteId()).Get();
				uiItems.at(destinationSlot) = std::make_unique<UIItem>(UIComponentArgs{ deviceResources, uiComponents, [posX, posY](const float, const float) { return XMFLOAT2{ posX + 2.0f, posY + 2.0f }; }, uiLayer, zIndex + 1 }, eventHandler, socketManager, item->GetId(), item->GetName(), item->GetDescription());
				AddChildComponent(*uiItems.at(destinationSlot).get());
				uiItems.at(destinationSlot)->Initialize(vertexShader, pixelShader, texture, grayTexture, highlightBrush, vertexShaderBuffer, vertexShaderSize, bodyBrush, borderBrush, textBrush, textFormatTitle, textFormatDescription);
				uiItems.at(destinationSlot)->CreatePositionDependentResources();
				uiItems.at(destinationSlot)->isVisible = isVisible;

				std::unique_ptr<Event> e = std::make_unique<Event>(EventType::ReorderUIComponents);
				eventHandler.QueueEvent(e);
			}

			break;
		}
		case EventType::WindowResize:
		{
			ReinitializeGeometry();

			break;
		}
		case EventType::UIItemDropped:
		{
			const auto derivedEvent = (UIItemDroppedEvent*)event;

			const auto worldPos = GetWorldPosition();
			const auto slot = Utility::GetInventoryIndex(worldPos.x, worldPos.y, derivedEvent->mousePosX, derivedEvent->mousePosY);

			if (draggingSlot >= 0 && slot >= 0)
			{
				std::vector<std::string> args{ std::to_string(draggingSlot), std::to_string(slot) };
				socketManager.SendPacket(OpCode::MoveItem, args);
			}

			break;
		}
		case EventType::MoveItemSuccess:
		{
			const auto derivedEvent = (MoveItemSuccessEvent*)event;
			const auto draggingSlot = derivedEvent->draggingSlot;
			const auto slot = derivedEvent->slot;

			auto swapped = false;
			if (slot >= 0)
			{
				auto row = slot / 4;
				auto col = slot % 4;
				auto posX = 5.0f + (col * 45.0f);
				auto posY = 25.0f + (row * 45.0f);

				Item* tmpItem = nullptr;
				std::unique_ptr<UIItem> tmpUIItem;
				if (items.at(slot))
				{
					swapped = true;
					tmpItem = items.at(slot);
					tmpUIItem = std::move(uiItems.at(slot));
				}

				items.at(slot) = items.at(draggingSlot);

				uiItems.at(slot) = std::move(uiItems.at(draggingSlot));
				uiItems.at(slot)->SetLocalPosition(XMFLOAT2{ posX + 2.0f, posY + 2.0f });
				uiItems.at(slot)->CreatePositionDependentResources();

				if (swapped)
				{
					row = draggingSlot / 4;
					col = draggingSlot % 4;
					posX = 5.0f + (col * 45.0f);
					posY = 25.0f + (row * 45.0f);

					items.at(draggingSlot) = tmpItem;
					uiItems.at(draggingSlot) = std::move(tmpUIItem);
					uiItems.at(draggingSlot)->SetLocalPosition(XMFLOAT2{ posX + 2.0f, posY + 2.0f });
					uiItems.at(draggingSlot)->CreatePositionDependentResources();
				}

				uiItems.at(slot)->SetTooltipAsVisible();

				// it's important that UIAbilities receive certain events (mouse clicks for example) before other
				// UI elements, so we reorder here to make sure the UIComponents are in the right order.
				std::unique_ptr<Event> e = std::make_unique<Event>(EventType::ReorderUIComponents);
				eventHandler.QueueEvent(e);
			}

			if (draggingSlot >= 0)
			{
				if (slot >= 0 && slot != draggingSlot && !swapped)
				{
					items.at(draggingSlot) = nullptr;
					uiItems.at(draggingSlot).reset();
				}

				this->draggingSlot = -1;
			}

			break;
		}
		case EventType::StartDraggingUIItem:
		{
			const auto derivedEvent = (StartDraggingUIItemEvent*)event;

			draggingSlot = derivedEvent->slot;

			break;
		}
	}
	
	return false;
}

void UIInventory::ReinitializeGeometry()
{
	const auto d2dFactory = deviceResources->GetD2DFactory();
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
