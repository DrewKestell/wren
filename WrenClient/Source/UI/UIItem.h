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
	const std::string& name;
	const std::string& description;
	ID2D1SolidColorBrush* highlightBrush;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* grayTexture;
	ID2D1SolidColorBrush* bodyBrush;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormatTitle;
	IDWriteTextFormat* textFormatDescription;
	const BYTE* vertexShaderBuffer;
	const int vertexShaderSize;
	const XMMATRIX projectionTransform;
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
		ID2D1SolidColorBrush* bodyBrush,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* textBrush,
		IDWriteTextFormat* textFormatTitle,
		IDWriteTextFormat* textFormatDescription,
		const bool isDragging = false,
		const float mousePosX = 0.0f,
		const float mousePosY = 0.0f);

	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
};