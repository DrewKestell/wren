#pragma once

#include "UIComponent.h"
#include <EventHandling/EventHandler.h>
#include "../Models/Item.h"
#include "UIItem.h"
#include <Constants.h>

class UIInventory : public UIComponent
{
	EventHandler& eventHandler;
	ClientSocketManager& socketManager;
	std::vector<std::unique_ptr<Item>>& allItems;
	std::vector<ComPtr<ID3D11ShaderResourceView>> allTextures;
	std::vector<Item*> items = std::vector<Item*>(INVENTORY_SIZE, nullptr);
	std::vector<std::unique_ptr<UIItem>> uiItems = std::vector<std::unique_ptr<UIItem>>(INVENTORY_SIZE);
	ComPtr<ID2D1RectangleGeometry> geometry[INVENTORY_SIZE];
	ID2D1SolidColorBrush* brush;
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

	void ReinitializeGeometry();
public:
	float clientWidth;
	float clientHeight;
	int playerId;
	XMMATRIX projectionTransform;

	UIInventory(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		ClientSocketManager& socketManager,
		std::vector<std::unique_ptr<Item>>& allItems,
		std::vector<ComPtr<ID3D11ShaderResourceView>> allTextures,
		ID2D1SolidColorBrush* brush,
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
		IDWriteTextFormat* textFormatDescription);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIItemDragBehavior() const override;
};