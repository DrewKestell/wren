#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include <Models/Ability.h>
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

static constexpr auto SPRITE_WIDTH = 36.0f;
static constexpr auto HIGHLIGHT_WIDTH = 40.0f;
static constexpr auto ABILITY_HEADER_WIDTH = 200.0f;
static constexpr auto ABILITY_HEADER_HEIGHT = 30.0f;
static constexpr auto ABILITY_BORDER_WIDTH = 40.0f;

class UIAbility : public UIComponent
{
	EventHandler& eventHandler;
	Ability* ability;
	bool toggled;
	bool isDragging;
	float lastDragX;
	float lastDragY;
	IDWriteTextFormat* headerTextFormat{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* headerBrush{ nullptr };
	ID2D1SolidColorBrush* highlightBrush{ nullptr };
	ID2D1SolidColorBrush* abilityPressedBrush{ nullptr };
	ID2D1SolidColorBrush* abilityToggledBrush{ nullptr };
	const BYTE* vertexShaderBuffer{ nullptr };
	int vertexShaderSize{ 0 };
	ComPtr<IDWriteTextLayout> headerTextLayout;
	ComPtr<ID2D1RectangleGeometry> borderGeometry;
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	ComPtr<ID2D1RectangleGeometry> toggledGeometry;
	std::shared_ptr<Sprite> sprite;
	UIAbility* abilityCopy{ nullptr };
	bool isHovered{ false };
	bool isPressed{ false };
	bool isToggled{ false };

public:
	UIAbility(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		Ability* ability,
		const bool toggled,
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);
	void Initialize(
		IDWriteTextFormat* headerTextFormat,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11ShaderResourceView* texture,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* headerBrush,
		ID2D1SolidColorBrush* highlightBrush,
		ID2D1SolidColorBrush* abilityPressedBrush,
		ID2D1SolidColorBrush* abilityToggledBrush,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize);
	void Draw() override;
	void DrawHeadersAndBorders();
	const bool HandleEvent(const Event* const event) override;
	void CreatePositionDependentResources();
};