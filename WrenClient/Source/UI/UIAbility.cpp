#include "stdafx.h"
#include "UIAbility.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UIAbility::UIAbility(std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const int abilityId,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	ID3D11Device* d3dDevice,
	const float originX,
	const float originY)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  abilityId{ abilityId }
{
	sprite = std::make_unique<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, d3dDevice, originX, originY, SPRITE_WIDTH, SPRITE_WIDTH);
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + HIGHLIGHT_WIDTH, position.y + HIGHLIGHT_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());
}

void UIAbility::Draw()
{
	if (!isVisible) return;

	
}

const bool UIAbility::HandleEvent(const Event* const event)
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