#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIAbility : public UIComponent
{
	const float SPRITE_WIDTH = 38.0f;
	const float HIGHLIGHT_WIDTH = 40.0f;
	const int abilityId{ 0 };
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	std::shared_ptr<Sprite> sprite;
	ID2D1DeviceContext1* d2dDeviceContext{ nullptr };
	ID2D1SolidColorBrush* highlightBrush;
	ID3D11DeviceContext* d3dDeviceContext;
	const XMMATRIX projectionTransform;
public:
	UIAbility(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		const int abilityId,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory,
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dDeviceContext,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11ShaderResourceView* texture,
		ID2D1SolidColorBrush* highlightBrush,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		const float worldPosX,
		const float worldPosY,
		const float clientWidth,
		const float clientHeight,
		const XMMATRIX projectionTransform);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};