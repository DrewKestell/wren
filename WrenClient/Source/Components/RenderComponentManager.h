#pragma once

#include "RenderComponent.h"
#include "ObjectManager.h"
#include "EventHandling/Observer.h"

constexpr unsigned int MAX_RENDERCOMPONENTS_SIZE = 100000;

class RenderComponentManager : public Observer
{
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	std::map<int, int> idIndexMap;
	RenderComponent renderComponents[MAX_RENDERCOMPONENTS_SIZE];
	int renderComponentIndex{ 0 };
public:
	RenderComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	void Render(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag);
	RenderComponent& CreateRenderComponent(const int gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void DeleteRenderComponent(const int renderComponentId);
	RenderComponent& GetRenderComponentById(const int renderComponentId);
	virtual const bool HandleEvent(const Event* const event);
	~RenderComponentManager();
};