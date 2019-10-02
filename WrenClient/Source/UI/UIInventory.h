#pragma once

#include "UIComponent.h"
#include <EventHandling/EventHandler.h>
#include "../Models/Item.h"
#include "UIItem.h"

constexpr auto INVENTORY_SIZE = 16;

class UIInventory : public UIComponent
{
	EventHandler& eventHandler;
	std::vector<std::unique_ptr<Item>>& allItems;
	std::vector<ComPtr<ID3D11ShaderResourceView>> allTextures;
	std::vector<Item*> items{ INVENTORY_SIZE, nullptr };
	std::vector<std::unique_ptr<UIItem>> uiItems{ INVENTORY_SIZE };
	ComPtr<ID2D1RectangleGeometry> geometry[INVENTORY_SIZE];
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

	void ReinitializeGeometry();
public:
	float clientWidth;
	float clientHeight;
	XMMATRIX projectionTransform;

	UIInventory(
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
		const float clientHeight);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIItemDragBehavior() const override;
};