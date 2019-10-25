#include "stdafx.h"
#include "UILootContainer.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "../Events/DoubleLeftMouseDownEvent.h"
#include "EventHandling/Events/LootItemSuccessEvent.h"

UILootContainer::UILootContainer(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler,
	ClientSocketManager& socketManager,
	StatsComponentManager& statsComponentManager,
	InventoryComponentManager& inventoryComponentManager,
	std::vector<std::unique_ptr<Item>>& allItems)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler },
	  socketManager{ socketManager },
	  statsComponentManager{ statsComponentManager },
	  inventoryComponentManager{ inventoryComponentManager },
	  allItems{ allItems }
{
}

void UILootContainer::Initialize(
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
	IDWriteTextFormat* textFormatDescription,
	std::vector<ComPtr<ID3D11ShaderResourceView>>* allTextures
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
	this->allTextures = allTextures;
}

void UILootContainer::Draw()
{
	if (!isVisible) return;

	const auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();

	for (auto i = 0; i < 12; i++)
		d2dDeviceContext->DrawGeometry(geometry[i].Get(), brush, 2.0f);
}

const bool UILootContainer::HandleEvent(const Event* const event)
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
		case EventType::DoubleLeftMouseDown:
		{
			const auto derivedEvent = (DoubleLeftMouseDownEvent*)event;

			if (derivedEvent->clickedObject && !isVisible)
			{
				const auto clickedObject = derivedEvent->clickedObject;
				const auto clickedObjectId = clickedObject->GetId();
				const StatsComponent& statsComponent = statsComponentManager.GetComponentById(clickedObject->statsComponentId);
				
				if (!statsComponent.alive && clickedObjectId != currentGameObjectId)
				{
					currentGameObjectId = clickedObjectId;

					// reset state
					items.clear();
					uiItems.clear();
					ClearChildren();

					// initialize UIItems
					const InventoryComponent& inventoryComponent = inventoryComponentManager.GetComponentById(clickedObject->inventoryComponentId);
					for (auto i = 0; i < INVENTORY_SIZE; i++)
					{
						const auto itemId = inventoryComponent.itemIds.at(i);
						if (itemId >= 0)
						{
							const auto item = allItems.at(itemId - 1).get();
							items.push_back(item);

							const auto xOffset = 5.0f + ((i % 3) * 45.0f);
							const auto yOffset = 25.0f + ((i / 3) * 45.0f);
							const auto texture = allTextures->at(item->GetSpriteId()).Get();
							const auto grayTexture = allTextures->at(item->GetGraySpriteId()).Get();
							uiItems.push_back(std::make_unique<UIItem>(UIComponentArgs{ deviceResources, uiComponents, [xOffset, yOffset](const float, const float) { return XMFLOAT2{ xOffset + 2.0f, yOffset + 2.0f }; }, uiLayer, 3 }, eventHandler, socketManager, itemId, item->GetName(), item->GetDescription()));
							AddChildComponent(*uiItems.at(i).get());
							uiItems.at(i)->Initialize(vertexShader, pixelShader, texture, grayTexture, highlightBrush, vertexShaderBuffer, vertexShaderSize, bodyBrush, borderBrush, textBrush, textFormatTitle, textFormatDescription);
							uiItems.at(i)->CreatePositionDependentResources();
						}
					}

					std::unique_ptr<Event> e = std::make_unique<Event>(EventType::ReorderUIComponents);
					eventHandler.QueueEvent(e);
				}
			}

			break;
		}
		case EventType::LootItemSuccess:
		{
			const auto derivedEvent = (LootItemSuccessEvent*)event;

			if (isVisible && derivedEvent->gameObjectId == currentGameObjectId)
			{
				const auto slot = derivedEvent->slot;

				RemoveChildComponent(uiItems.at(slot).get());
				items.at(slot) = nullptr;
				uiItems.at(slot) = nullptr;
			}
			
			break;
		}
		case EventType::WindowResize:
		{
			ReinitializeGeometry();

			break;
		}
	}

	return false;
}

void UILootContainer::ReinitializeGeometry()
{
	const auto d2dFactory = deviceResources->GetD2DFactory();
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

const std::string UILootContainer::GetUIItemRightClickBehavior() const
{
	return "LOOT";
}