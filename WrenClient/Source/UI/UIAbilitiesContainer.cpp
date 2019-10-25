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
	std::vector<ComPtr<ID3D11ShaderResourceView>>* allTextures)
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
}

void UIAbilitiesContainer::Draw()
{
	if (!isVisible) return;

	for (auto i = 0; i < uiAbilities.size(); i++)
		uiAbilities.at(i)->DrawHeadersAndBorders();
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
		case EventType::WindowResize:
		{

			// TODO

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

		uiAbility->Initialize(headerTextFormat, vertexShader, pixelShader, texture.Get(), borderBrush, headerBrush, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize);		
		uiAbility->CreatePositionDependentResources();
	}
}
