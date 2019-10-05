#pragma once

#include "RenderComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"
#include <Components/ComponentManager.h>

class RenderComponentManager : public ComponentManager<RenderComponent, 100000>
{
public:
	RenderComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	RenderComponent& CreateRenderComponent(const int gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Update(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag);
};