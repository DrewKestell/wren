#pragma once

#include "RenderComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"

static const unsigned int MAX_RENDERCOMPONENTS_SIZE = 100000;

class RenderComponentManager : public Observer
{
	std::map<unsigned int, unsigned int> idIndexMap;
	RenderComponent renderComponents[MAX_RENDERCOMPONENTS_SIZE];
	unsigned int renderComponentIndex{ 0 };
	ObjectManager& objectManager;
public:
	RenderComponentManager(ObjectManager& objectManager);
	void Render(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag);
	RenderComponent& CreateRenderComponent(const long gameObjectId, std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void DeleteRenderComponent(const unsigned int renderComponentId);
	virtual const bool HandleEvent(const Event* const event);
	~RenderComponentManager();
};