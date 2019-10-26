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
	ID2D1SolidColorBrush* borderBrush{ nullptr };
	ID2D1SolidColorBrush* highlightBrush{ nullptr };
	ID2D1SolidColorBrush* headerBrush{ nullptr };
	ID2D1SolidColorBrush* abilityPressedBrush{ nullptr };
	ID2D1SolidColorBrush* abilityToggledBrush{ nullptr };
	IDWriteTextFormat* headerTextFormat{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	const BYTE* vertexShaderBuffer{ nullptr };
	int vertexShaderSize{ 0 };
	std::vector<ComPtr<ID3D11ShaderResourceView>>* allTextures{ nullptr };
	ID2D1SolidColorBrush* tooltipBodyBrush{ nullptr };
	ID2D1SolidColorBrush* tooltipBorderBrush{ nullptr };
	ID2D1SolidColorBrush* tooltipTextBrush{ nullptr };
	IDWriteTextFormat* tooltipTextFormatTitle{ nullptr };
	IDWriteTextFormat* tooltipTextFormatDescription{ nullptr };
	std::vector<std::unique_ptr<Ability>>* abilities;
	std::vector<std::shared_ptr<UIAbility>> uiAbilities;

	void CreateAbilities();
	void InitializeAbilities();
	
public:
	UIAbilitiesContainer(UIComponentArgs uiComponentArgs, EventHandler& eventHandler);
	void Initialize(
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
		std::vector<ComPtr<ID3D11ShaderResourceView>>* allTextures,
		ID2D1SolidColorBrush* tooltipBodyBrush,
		ID2D1SolidColorBrush* tooltipBorderBrush,
		ID2D1SolidColorBrush* tooltipTextBrush,
		IDWriteTextFormat* tooltipTextFormatTitle,
		IDWriteTextFormat* tooltipTextFormatDescription);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIAbilityDragBehavior() const override;
	void SetAbilities(std::vector<std::unique_ptr<Ability>>* abilities);
};