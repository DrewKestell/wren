#include "stdafx.h"
#include "UIAbilitiesContainer.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

using namespace DX;

UIAbilitiesContainer::UIAbilitiesContainer(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	IDWriteFactory2* writeFactory,
	ID2D1SolidColorBrush* borderBrush,
	ID2D1SolidColorBrush* highlightBrush,
	ID2D1SolidColorBrush* headerBrush,
	IDWriteTextFormat* headerTextFormat,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const XMMATRIX projectionTransform,
	const float clientWidth,
	const float clientHeight)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  d2dDeviceContext{ d2dDeviceContext },
	  d2dFactory{ d2dFactory },
	  d3dDevice{ d3dDevice },
	  d3dDeviceContext{ d3dDeviceContext },
	  writeFactory{ writeFactory },
	  borderBrush{ borderBrush },
	  highlightBrush{ highlightBrush },
	  headerBrush{ headerBrush },
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

	for (auto i = 0; i < headers.size(); i++)
	{
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(worldPos.x + 12.0f, worldPos.y + 30.0 + (i * 60.0f)), headers.at(i).Get(), headerBrush);
		d2dDeviceContext->DrawGeometry(borderGeometries.at(i).Get(), borderBrush, 2.0f);
		// if mouse hover, draw highlight
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
	d2dFactory->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(positionX, positionY, positionX + BORDER_WIDTH, positionY + BORDER_WIDTH), 3.0f, 3.0f), borderGeometry.ReleaseAndGetAddressOf());
	borderGeometries.push_back(borderGeometry);

	// create UIAbility
	auto uiAbility = std::shared_ptr<UIAbility>(new UIAbility(uiComponents, XMFLOAT3{ xOffset + 2.0f, yOffset + 2.0f, 0.0f }, scale, uiLayer, ability->abilityId, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, vertexShader, pixelShader, texture, highlightBrush, vertexShaderBuffer, vertexShaderSize, positionX + 2.0f, positionY + 2.0f, clientWidth, clientHeight, projectionTransform));
	uiAbilities.push_back(uiAbility);
	this->AddChildComponent(*uiAbility);
}