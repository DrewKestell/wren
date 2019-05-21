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
	const float originY,
	ID3D11DeviceContext* d3dDeviceContext,
	const XMMATRIX projectionTransform)
	: UIComponent(uiComponents, position, scale, uiLayer),
	  abilityId{ abilityId },
	  d3dDeviceContext{ d3dDeviceContext },
	  projectionTransform{ projectionTransform }
{
	// this doesn't work, because when the constructor is called, the panel parent doesn't exist yet - so the position
	// will be wrong. 
	auto worldPosition = GetWorldPosition();
	auto pos = XMFLOAT3{ worldPosition.x + 20.0f, worldPosition.y + 20.0f, 0.0f };
	FXMVECTOR v = XMLoadFloat3(&pos);
	CXMMATRIX view = XMMatrixIdentity();
	CXMMATRIX world = XMMatrixIdentity();

	// this is good though!
	auto res = XMVector3Unproject(v, 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1000.0f, projectionTransform, view, world);
	XMFLOAT3 vec;
	XMStoreFloat3(&vec, res);

	sprite = std::make_unique<Sprite>(vertexShader, pixelShader, texture, vertexShaderBuffer, vertexShaderSize, d3dDevice, vec.x, vec.y , SPRITE_WIDTH, SPRITE_WIDTH);
	d2dFactory->CreateRectangleGeometry(D2D1::RectF(position.x, position.y, position.x + HIGHLIGHT_WIDTH, position.y + HIGHLIGHT_WIDTH), highlightGeometry.ReleaseAndGetAddressOf());
}

void UIAbility::Draw()
{
	if (!isVisible) return;

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