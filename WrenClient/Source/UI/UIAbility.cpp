#include "stdafx.h"
#include "UIAbility.h"
#include "Layer.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"

UIAbility::UIAbility(
	std::vector<UIComponent*>& uiComponents,
	const XMFLOAT3 position,
	const XMFLOAT3 scale,
	const Layer uiLayer,
	const int abilityId,
	ID2D1DeviceContext1* d2dDeviceContext,
	ID2D1Factory2* d2dFactory,
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader,
	ID3D11ShaderResourceView* texture,
	ID2D1SolidColorBrush* highlightBrush,
	const BYTE* vertexShaderBuffer,
	const int vertexShaderSize,
	const float worldPosX,
	const float worldPosY,
	const float clientWidth,
	const float clientHeight,
	const XMMATRIX projectionTransform)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  abilityId{ abilityId },
	  d2dDeviceContext{ d2dDeviceContext },
	  d3dDeviceContext{ d3dDeviceContext },
	  highlightBrush{ highlightBrush },
	  projectionTransform{ projectionTransform }
{
	// create highlight
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(worldPosX, worldPosY , worldPosX + SPRITE_WIDTH, worldPosX + SPRITE_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());

	// create UIAbility
	auto pos = XMFLOAT3{ worldPosX + 18.0f, worldPosY + 18.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	// this is good though!
	auto res = XMVector3Unproject(v, 0.0f, 0.0f, clientWidth, clientHeight, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);
	sprite = std::make_shared<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, d3dDevice, vec.x, vec.y, SPRITE_WIDTH, SPRITE_WIDTH);
}

void UIAbility::Draw()
{
	if (!isVisible) return;

	// if hover, draw highlight
	//d2dDeviceContext->DrawGeometry(highlightGeometry.Get(), highlightBrush, 2.0f);
	sprite->Draw(d3dDeviceContext, projectionTransform);
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