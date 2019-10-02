#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/EventHandler.h"
#include "../ClientSocketManager.h"

class UIItem : public UIComponent
{
	EventHandler& eventHandler;
	ClientSocketManager& socketManager;
	const float SPRITE_SIZE{ 36.0f };
	bool isHovered{ false };
	bool isPressed{ false };
	bool isDragging;
	float lastDragX;
	float lastDragY;
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	std::shared_ptr<Sprite> sprite;
	const float clientWidth;
	const float clientHeight;
	const int itemId;
	ID2D1SolidColorBrush* highlightBrush;
	ID2D1Factory2* d2dFactory;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dDeviceContext;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* grayTexture;
	const BYTE* vertexShaderBuffer;
	const int vertexShaderSize;
	const XMMATRIX projectionTransform;
	UIItem* itemCopy{ nullptr };

public:
	UIItem(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		ClientSocketManager& socketManager,
		const int itemId,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory,
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dDeviceContext,
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
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};