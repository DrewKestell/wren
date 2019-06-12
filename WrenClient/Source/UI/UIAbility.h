#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIAbility : public UIComponent
{
	const float SPRITE_WIDTH{ 36.0f };
	const float HIGHLIGHT_WIDTH{ 36.0f };
	bool isHovered{ false };
	bool isPressed{ false };
	bool isDragging;
	float lastDragX;
	float lastDragY;
	const int abilityId;
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	std::shared_ptr<Sprite> sprite;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;
	ID3D11Device* d3dDevice;
	const float clientWidth;
	const float clientHeight;
	ID2D1SolidColorBrush* highlightBrush;
	ID2D1SolidColorBrush* abilityPressedBrush;
	ID3D11DeviceContext* d3dDeviceContext;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	const BYTE* vertexShaderBuffer;
	const int vertexShaderSize;
	const XMMATRIX projectionTransform;
	UIAbility* abilityCopy{ nullptr };
public:
	UIAbility(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
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
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void DrawSprite();
};