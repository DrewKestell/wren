#include "stdafx.h"
#include "UIAbilitiesContainer.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIAbilitiesContainer::UIAbilitiesContainer(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT2 position,
	const Layer uiLayer,
	const int zIndex,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	IDWriteFactory2* writeFactory,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* highlightBrush,
	ID2D1SolidColorBrush* headerBrush,
	ID2D1SolidColorBrush* abilityPressedBrush,
	IDWriteTextFormat* headerTextFormat,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const XMMATRIX projectionTransform,
	const float clientWidth,
	const float clientHeight)
	: UIComponent(uiComponents, position, uiLayer, zIndex),
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  d3dDevice{ d3dDevice },
	  d3dDeviceContext{ d3dDeviceContext },
	  writeFactory{ writeFactory },
	  borderBrush{ borderBrush },
	  highlightBrush{ highlightBrush },
	  headerBrush{ headerBrush },
	  abilityPressedBrush{ abilityPressedBrush },
	  headerTextFormat{ headerTextFormat },
	  vertexShader{ vertexShader },
	  pixelShader{ pixelShader },
	  vertexShaderBuffer{ vertexShaderBuffer },
	  vertexShaderSize{ vertexShaderSize },
	  projectionTransform{ projectionTransform },
	  clientWidth{ clientWidth },
	  clientHeight{ clientHeight }
{
}

void UIAbilitiesContainer::Draw()
{
	if (!isVisible) return;

	const auto worldPos = GetWorldPosition();
	const auto initialSize = headers.size();

	for (auto i = 0; i < headers.size(); i++)
	{
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(worldPos.x + 12.0f, worldPos.y + 30.0f + (i * 60.0f)), headers.at(i).Get(), headerBrush);
		// if mouse hover, draw highlight

		// Draw Borders
		borderGeometries.clear();
		ComPtr<ID2D1RectangleGeometry> borderGeometry;
		auto xOffset = 12.0f;
		auto yOffset = 50.0f + (borderGeometries.size() * 60.0f);
		auto positionX = worldPos.x + xOffset;
		auto positionY = worldPos.y + yOffset;
		d2dFactory->CreateRectangleGeometry(D2D1::RectF(positionX, positionY, positionX + BORDER_WIDTH, positionY + BORDER_WIDTH), borderGeometry.ReleaseAndGetAddressOf());
		borderGeometries.push_back(borderGeometry);
		d2dDeviceContext->DrawGeometry(borderGeometries.at(i).Get(), borderBrush, 2.0f);
	}
}

const bool UIAbilitiesContainer::HandleEvent(const Event* const event)
{
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

void UIAbilitiesContainer::AddAbility(Ability* ability, ID3D11ShaderResourceView* texture)
{
	// save the ability so we can rebuild this when we resize the window. might not need this!
	abilities.push_back(ability);

	auto initialSize = headers.size();

	// construct the header
	ComPtr<IDWriteTextLayout> headerTextLayout;
	std::wostringstream buttonText;
	buttonText << ability->name.c_str();
	ThrowIfFailed(writeFactory->CreateTextLayout(
		buttonText.str().c_str(),
		(UINT32)buttonText.str().size(),
		headerTextFormat,
		HEADER_WIDTH,
		HEADER_HEIGHT,
		headerTextLayout.ReleaseAndGetAddressOf())
	);
	headers.push_back(headerTextLayout);

	// create border
	const auto worldPos = GetWorldPosition();
	ComPtr<ID2D1RoundedRectangleGeometry> borderGeometry;
	auto xOffset = 12.0f;
	auto yOffset = 50.0f + (initialSize * 60.0f);
	auto positionX = worldPos.x + xOffset;
	auto positionY = worldPos.y + yOffset;

	// create UIAbility
	auto uiAbility = std::shared_ptr<UIAbility>(new UIAbility(uiComponents, XMFLOAT2{ xOffset + 2.0f, yOffset + 2.0f }, uiLayer, 2, ability->abilityId, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, vertexShader, pixelShader, texture, highlightBrush, abilityPressedBrush, vertexShaderBuffer, vertexShaderSize, positionX + 2.0f, positionY + 2.0f, clientWidth, clientHeight, projectionTransform));
	uiAbilities.push_back(uiAbility);
	this->AddChildComponent(*uiAbility);
}

void UIAbilitiesContainer::DrawSprites()
{
	for (auto i = 0; i < uiAbilities.size(); i++)
	{
		uiAbilities.at(i)->DrawSprite();
	}
}

const std::string UIAbilitiesContainer::GetUIAbilityDragBehavior() const
{
	return "COPY";
}

void UIAbilitiesContainer::ClearAbilities()
{
	for (auto i = 0; i < abilities.size(); i++)
		delete abilities.at(i);
	abilities.clear();
	headers.clear();
	borderGeometries.clear();
	uiAbilities.clear();
}