#pragma once

#include "UIComponent.h"
#include "../Sprite.h"
#include "EventHandling/EventHandler.h"
#include "../ClientSocketManager.h"
#include "UITooltip.h"

static constexpr auto SPRITE_SIZE = 36.0f;
class UIItem : public UIComponent
{
	EventHandler& eventHandler;
	ClientSocketManager& socketManager;
	const int itemId;
	const std::string& name;
	const std::string& description;
	bool isDragging;
	float lastDragX;
	float lastDragY;
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	ID3D11ShaderResourceView* grayTexture{ nullptr };
	ID2D1SolidColorBrush* highlightBrush{ nullptr };
	const BYTE* vertexShaderBuffer{ nullptr };
	int vertexShaderSize{ 0 };
	ID2D1SolidColorBrush* bodyBrush{ nullptr };
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* textBrush{ nullptr };
	IDWriteTextFormat* textFormatTitle{ nullptr };
	IDWriteTextFormat* textFormatDescription{ nullptr };
	bool isHovered{ false };
	bool isPressed{ false };
	ComPtr<ID2D1RectangleGeometry> highlightGeometry;
	std::shared_ptr<Sprite> sprite;
	UIItem* itemCopy{ nullptr };
	std::unique_ptr<UITooltip> tooltip;

public:
	UIItem(
		UIComponentArgs uiComponentArgs,
		EventHandler& eventHandler,
		ClientSocketManager& socketManager,
		const int itemId,
		const std::string& name,
		const std::string& description,
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);
	void Initialize(
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* grayTexture,
		ID2D1SolidColorBrush* highlightBrush,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};