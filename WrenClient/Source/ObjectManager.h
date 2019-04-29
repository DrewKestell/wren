#pragma once

#include "GameObject.h"
#include "RenderComponent.h"

static const unsigned int MAX_GAMEOBJECTS_SIZE = 100000;
static const unsigned int MAX_RENDERCOMPONENTS_SIZE = 100000;

class ObjectManager
{
	GameObject gameObjects[MAX_GAMEOBJECTS_SIZE];
	RenderComponent renderComponents[MAX_RENDERCOMPONENTS_SIZE];
	unsigned int gameObjectIndex{ 0 };
	unsigned int renderComponentIndex{ 0 };
public:
	void Render(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag);
	GameObject& CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale);
	RenderComponent& CreateRenderComponent(const unsigned int gameObjectId, std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void DeleteGameObject(const unsigned int gameObjectId);
	void DeleteRenderComponent(const unsigned int renderComponentId);
};