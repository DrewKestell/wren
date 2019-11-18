#include "stdafx.h"
#include "UIAbilitiesContainer.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UIAbilitiesContainer::UIAbilitiesContainer(UIComponentArgs uiComponentArgs, EventHandler& eventHandler)
	: UIComponent(uiComponentArgs),
	  eventHandler{ eventHandler }
{
}

void UIAbilitiesContainer::Initialize(
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
	IDWriteTextFormat* tooltipTextFormatDescription)
{
	this->borderBrush = borderBrush;
	this->highlightBrush = highlightBrush;
	this->headerBrush = headerBrush;
	this->abilityPressedBrush = abilityPressedBrush;
	this->abilityToggledBrush = abilityToggledBrush;
	this->headerTextFormat = headerTextFormat;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->vertexShaderBuffer = vertexShaderBuffer;
	this->vertexShaderSize = vertexShaderSize;
	this->allTextures = allTextures;
	this->tooltipBodyBrush = tooltipBodyBrush;
	this->tooltipBorderBrush = tooltipBorderBrush;
	this->tooltipTextBrush = tooltipTextBrush;
	this->tooltipTextFormatTitle = tooltipTextFormatTitle;
	this->tooltipTextFormatDescription = tooltipTextFormatDescription;
}

void UIAbilitiesContainer::Draw()
{
	if (!isVisible) return;

	for (auto i = 0; i < uiAbilities.size(); i++)
	{
		uiAbilities.at(i)->DrawHeadersAndBorder();
		uiAbilities.at(i)->DrawHoverAndToggledBorders();
	}
}

const bool UIAbilitiesContainer::HandleEvent(const Event* const event)
{
	// first pass the event to UIComponent base so it can reset localPosition based on new client dimensions
	UIComponent::HandleEvent(event);

	const auto type = event->type;
	switch (type)
	{
		case EventType::ChangeActiveLayer:
		{
			const auto derivedEvent = (ChangeActiveLayerEvent*)event;

			if (derivedEvent->layer == uiLayer && GetParent() == nullptr)
				isVisible = true;
			else
				isVisible = false;

			break;
		}
	}

	return false;
}

const std::string UIAbilitiesContainer::GetUIAbilityDragBehavior() const
{
	return "COPY";
}

void UIAbilitiesContainer::SetAbilities(std::vector<std::unique_ptr<Ability>>* abilities)
{
	uiAbilities.clear();
	ClearChildren();

	this->abilities = abilities;
	CreateAbilities();
	InitializeAbilities();
}

void UIAbilitiesContainer::CreateAbilities()
{
	for (auto i = 0; i < abilities->size(); i++)
	{
		Ability* ability = abilities->at(i).get();
		
		auto xOffset = 12.0f;
		auto yOffset = 30.0f + (i * 70.0f);

		auto uiAbility = std::shared_ptr<UIAbility>(new UIAbility(UIComponentArgs(deviceResources, uiComponents, [xOffset, yOffset](const float, const float) { return XMFLOAT2{ xOffset + 2.0f, yOffset + 2.0f }; }, uiLayer, zIndex + 1), eventHandler, ability, ability->toggled));
		uiAbility->isVisible = isVisible;
		this->AddChildComponent(*uiAbility);		
		uiAbilities.push_back(uiAbility);
	}
}

void UIAbilitiesContainer::InitializeAbilities()
{
	for (auto i = 0; i < abilities->size(); i++)
	{
		auto uiAbility = uiAbilities.at(i);
		auto texture = allTextures->at(abilities->at(i)->spriteId);

		uiAbility->Initialize(headerTextFormat, vertexShader, pixelShader, texture.Get(), borderBrush, headerBrush, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize, tooltipBodyBrush, tooltipBorderBrush, tooltipTextBrush, tooltipTextFormatTitle, tooltipTextFormatDescription);
		uiAbility->CreatePositionDependentResources();
	}
}

UIAbility* UIAbilitiesContainer::GetUIAbilityById(const int abilityId)
{
	for (auto i = 0; i < uiAbilities.size(); i++)
	{
		auto ability = uiAbilities.at(i)->GetAbility();;
		if (ability->abilityId == abilityId)
		{
			auto xOffset = 12.0f;
			auto yOffset = 30.0f + (i * 70.0f);

			auto copy = new UIAbility(UIComponentArgs(deviceResources, uiComponents, [xOffset, yOffset](const float, const float) { return XMFLOAT2{ xOffset + 2.0f, yOffset + 2.0f }; }, uiLayer, zIndex + 1), eventHandler, ability, ability->toggled);
			auto texture = allTextures->at(ability->spriteId);
			copy->Initialize(headerTextFormat, vertexShader, pixelShader, texture.Get(), borderBrush, headerBrush, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize, tooltipBodyBrush, tooltipBorderBrush, tooltipTextBrush, tooltipTextFormatTitle, tooltipTextFormatDescription);
			return copy;
		}
	}
}
