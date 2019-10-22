#include "stdafx.h"
#include "UIAbilitiesContainer.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIAbilitiesContainer::UIAbilitiesContainer(
	UIComponentArgs uiComponentArgs,
	EventHandler& eventHandler)
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
	const int vertexShaderSize)
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
}

void UIAbilitiesContainer::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();
	const auto initialSize = headers.size();
	borderGeometries.clear();

	for (auto i = 0; i < headers.size(); i++)
	{
		deviceResources->GetD2DDeviceContext()->DrawTextLayout(D2D1::Point2F(worldPos.x + 12.0f, worldPos.y + 30.0f + (i * 70.0f)), headers.at(i).Get(), headerBrush);
		// if mouse hover, draw highlight

		// Draw Borders
		ComPtr<ID2D1RectangleGeometry> borderGeometry;
		auto xOffset = 12.0f;
		auto yOffset = 50.0f + (borderGeometries.size() * 70.0f);
		auto positionX = worldPos.x + xOffset;
		auto positionY = worldPos.y + yOffset;
		deviceResources->GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(positionX, positionY, positionX + ABILITIES_CONTAINER_BORDER_WIDTH, positionY + ABILITIES_CONTAINER_BORDER_WIDTH), borderGeometry.ReleaseAndGetAddressOf());
		borderGeometries.push_back(borderGeometry);
		deviceResources->GetD2DDeviceContext()->DrawGeometry(borderGeometries.at(i).Get(), borderBrush, 2.0f);
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
		case EventType::WindowResize:
		{

			// TODO

			break;
		}
	}

	return false;
}

void UIAbilitiesContainer::AddAbility(Ability* ability, ID3D11ShaderResourceView* texture)
{
	// save the ability so we can rebuild this when we resize the window. might not need this!
	abilities.push_back(ability);

	auto initialSize = headers.size();

	// construct the header
	ComPtr<IDWriteTextLayout> headerTextLayout;

	ThrowIfFailed(
		deviceResources->GetWriteFactory()->CreateTextLayout(
			Utility::s2ws(ability->name).c_str(),
			static_cast<unsigned int>(ability->name.size()),
			headerTextFormat,
			ABILITIES_CONTAINER_HEADER_WIDTH,
			ABILITIES_CONTAINER_HEADER_HEIGHT,
			headerTextLayout.ReleaseAndGetAddressOf()
		)
	);
	headers.push_back(headerTextLayout);

	// create border
	const auto worldPos = GetWorldPosition();
	ComPtr<ID2D1RoundedRectangleGeometry> borderGeometry;
	auto xOffset = 12.0f;
	auto yOffset = 50.0f + (initialSize * 70.0f);
	auto positionX = worldPos.x + xOffset;
	auto positionY = worldPos.y + yOffset;

	// create UIAbility
	auto uiAbility = std::shared_ptr<UIAbility>(new UIAbility(UIComponentArgs(deviceResources, uiComponents, [xOffset, yOffset](const float, const float) { return XMFLOAT2{ xOffset + 2.0f, yOffset + 2.0f }; }, uiLayer, zIndex + 1), eventHandler, ability->abilityId, ability->toggled));
	this->AddChildComponent(*uiAbility);
	uiAbility->Initialize(vertexShader, pixelShader, texture, highlightBrush, abilityPressedBrush, abilityToggledBrush, vertexShaderBuffer, vertexShaderSize);
	uiAbility->isVisible = isVisible;
	uiAbilities.push_back(uiAbility);
}

const std::string UIAbilitiesContainer::GetUIAbilityDragBehavior() const
{
	return "COPY";
}

void UIAbilitiesContainer::ClearAbilities()
{
	ClearChildren();
	abilities.clear();
	headers.clear();
	borderGeometries.clear();
	uiAbilities.clear();
}