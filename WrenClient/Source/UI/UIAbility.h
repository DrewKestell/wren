#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIAbility : public UIComponent
{
	const float ABILITY_SPRITE_WIDTH = 38.0f;
	const float HIGHLIGHT_WIDTH = 40.0f;
	const int abilityId{ 0 };
	std::unique_ptr<Sprite> sprite;
	ID2D1DeviceContext1* d2dDeviceContext{ nullptr };
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
public:
	UIAbility(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT3 position,
		const XMFLOAT3 scale,
		const Layer uiLayer,
		const int abilityId,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11ShaderResourceView* texture,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		ID3D11Device* device,
		const float originX,
		const float originY);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};