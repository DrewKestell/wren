#pragma once

#include <Models/Ability.h>
#include "UIComponent.h"
#include "UIAbility.h"
#include <EventHandling/EventHandler.h>
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIAbilitiesContainer : public UIComponent
{
	EventHandler& eventHandler;
	const float HEADER_WIDTH{ 200.0f };
	const float HEADER_HEIGHT{ 30.0f };
	const float BORDER_WIDTH{ 40.0f };
	const float SPRITE_WIDTH{ 36.0f };
	std::vector<Ability*> abilities;
	std::vector<ComPtr<IDWriteTextLayout>> headers;
	std::vector<ComPtr<ID2D1RectangleGeometry>> borderGeometries;
	std::vector<std::shared_ptr<UIAbility>> uiAbilities;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dDeviceContext;
	IDWriteFactory2* writeFactory;
	ID2D1SolidColorBrush* borderBrush;
	ID2D1SolidColorBrush* highlightBrush;
	ID2D1SolidColorBrush* headerBrush;
	ID2D1SolidColorBrush* abilityPressedBrush;
	ID2D1SolidColorBrush* abilityToggledBrush;
	IDWriteTextFormat* headerTextFormat;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	const BYTE* vertexShaderBuffer;
	const int vertexShaderSize;
public:
	float clientWidth;
	float clientHeight;
	XMMATRIX projectionTransform;

	UIAbilitiesContainer(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		EventHandler& eventHandler,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory,
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dDeviceContext,
		IDWriteFactory2* writeFactory,
		ID2D1SolidColorBrush* borderBrush,
		ID2D1SolidColorBrush* highlightBrush,
		ID2D1SolidColorBrush* headerBrush,
		ID2D1SolidColorBrush* abilityPressedBrush,
		ID2D1SolidColorBrush* abilityToggledBrush,
		IDWriteTextFormat* headerTextFormat,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		const BYTE* vertexShaderBuffer,
		const int vertexShaderSize,
		const XMMATRIX projectionTransform,
		const float clientWidth,
		const float clientHeight);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	void ClearAbilities();
	void AddAbility(Ability* ability, ID3D11ShaderResourceView* texture);
	const std::string GetUIAbilityDragBehavior() const override;
	void DrawSprites();
};