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
	std::vector<ComPtr<ID3D11ShaderResourceView>> allTextures;
	std::vector<Item*> items{ LOOT_CONTAINER_SIZE, nullptr };
	std::vector<std::unique_ptr<UIItem>> uiItems{ LOOT_CONTAINER_SIZE };
	int currentGameObjectId{ -1 };
	ComPtr<ID2D1RectangleGeometry> geometry[LOOT_CONTAINER_SIZE];
	ID2D1SolidColorBrush* brush;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dDeviceContext;
	ID2D1SolidColorBrush* highlightBrush;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	const BYTE* vertexShaderBuffer;
	const int vertexShaderSize;
	ID2D1SolidColorBrush* bodyBrush;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormatTitle;
	IDWriteTextFormat* textFormatDescription;
	IDWriteFactory2* writeFactory;

	void ReinitializeGeometry();

	friend class UIItem;
public:
	float clientWidth;
	float clientHeight;
	XMMATRIX projectionTransform;

	UILootContainer(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		ClientSocketManager& socketManager,
		StatsComponentManager& statsComponentManager,
		InventoryComponentManager& inventoryComponentManager,
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
		const float clientHeight,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription,
		IDWriteFactory2* writeFactory);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIItemRightClickBehavior() const override;
};