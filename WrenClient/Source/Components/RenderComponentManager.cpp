#include "stdafx.h"
#include "RenderComponentManager.h"

RenderComponentManager::RenderComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: ComponentManager(eventHandler, objectManager)
{
}

RenderComponent& RenderComponentManager::CreateRenderComponent(const int gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
{
	RenderComponent& renderComponent = CreateComponent(gameObjectId);

	renderComponent.mesh = mesh;
	renderComponent.vertexShader = vertexShader;
	renderComponent.pixelShader = pixelShader;
	renderComponent.texture = texture;

	return renderComponent;
}

void RenderComponentManager::Update(ID3D11DeviceContext* d3dContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag)
{
	for (auto i = 0; i < componentIndex; i++)
	{
		RenderComponent& renderComponent = components[i];
		GameObject& gameObject = objectManager.GetGameObjectById(renderComponent.gameObjectId);
		renderComponent.Draw(d3dContext, viewTransform, projectionTransform, updateLag, gameObject);
	}
}
