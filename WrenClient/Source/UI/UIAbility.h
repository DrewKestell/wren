#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

static constexpr auto SPRITE_WIDTH = 36.0f;
static constexpr auto HIGHLIGHT_WIDTH = 36.0f;

class UIAbility : public UIComponent
{
	EventHandler& eventHandler;
	const int abilityId;
	bool toggled;
	float clientWidth;
	float clientHeight;
	bool isDragging;
	float lastDragX;
	float lastDragY;
	bool isHovered{ false };
	bool isPressed{ false };
	bool isToggled{ false };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	ID2D1SolidColorBrush* highlightBrush{ nullptr };
	ID2D1SolidColorBrush* abilityPressedBrush{ nullptr };
	ID2D1SolidColorBrush* abilityToggledBrush{ nullptr };
	const BYTE* vertexShaderBuffer{ nullptr };
	int vertexShaderSize{ 0 };
	XMMATRIX projectionTransform{ XMMatrixIdentity() };
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	ComPtr<ID2D1RectangleGeometry> toggledGeometry;
	std::shared_ptr<Sprite> sprite;
	UIAbility* abilityCopy{ nullptr };
public:
	UIAbility(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		const int abilityId,
		const bool toggled,
		const float clientWidth,
		const float clientHeight,
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);
	void Initialize(
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11ShaderResourceView* texture,
		ID2D1SolidColorBrush* highlightBrush,
		ID2D1SolidColorBrush* abilityPressedBrush,
		ID2D1SolidColorBrush* abilityToggledBrush,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		const XMMATRIX projectionTransform
	);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};