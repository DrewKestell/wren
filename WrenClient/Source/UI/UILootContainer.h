#pragma once

#include "UIComponent.h"
#include <EventHandling/EventHandler.h>
#include "../Models/Item.h"
#include "UIItem.h"
#include <Components/StatsComponentManager.h>
#include <Components/InventoryComponentManager.h>

constexpr int LOOT_CONTAINER_SIZE = 12;

class UILootContainer : public UIComponent
{
	EventHandler& eventHandler;
	ClientSocketManager& socketManager;
	StatsComponentManager& statsComponentManager;
	InventoryComponentManager& inventoryComponentManager;
	std::vector<std::unique_ptr<Item>>& allItems;
	std::vector<ComPtr<ID3D11ShaderResourceView>>& allTextures;
	float clientWidth;
	float clientHeight;
	ID2D1SolidColorBrush* brush{ nullptr };
	ID2D1SolidColorBrush* highlightBrush{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	const BYTE* vertexShaderBuffer{ nullptr };
	int vertexShaderSize{ 0 };
	XMMATRIX projectionTransform{ XMMatrixIdentity() };
	ID2D1SolidColorBrush* bodyBrush{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* textBrush{ nullptr };
	IDWriteTextFormat* textFormatTitle{ nullptr };
	IDWriteTextFormat* textFormatDescription{ nullptr };
	std::vector<Item*> items{ LOOT_CONTAINER_SIZE, nullptr };
	std::vector<std::unique_ptr<UIItem>> uiItems{ LOOT_CONTAINER_SIZE };
	int currentGameObjectId{ -1 };
	ComPtr<ID2D1RectangleGeometry> geometry[LOOT_CONTAINER_SIZE];

	void ReinitializeGeometry();

	friend class UIItem;
public:
	UILootContainer(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		ClientSocketManager& socketManager,
		StatsComponentManager& statsComponentManager,
		InventoryComponentManager& inventoryComponentManager,
		std::vector<std::unique_ptr<Item>>& allItems,
		std::vector<ComPtr<ID3D11ShaderResourceView>>& allTextures,
		const float clientWidth,
		const float clientHeight);
	void Initialize(
		ID2D1SolidColorBrush* brush,
		ID2D1SolidColorBrush* highlightBrush,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		const XMMATRIX projectionTransform,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIItemRightClickBehavior() const override;
};