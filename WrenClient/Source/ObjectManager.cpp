#include "stdafx.h"
#include "ObjectManager.h"

void ObjectManager::Update(const float deltaTime)
{
	for (unsigned int i = 0; i < gameObjectIndex; i++)
		gameObjects[i].Update(deltaTime);
}

void ObjectManager::Render(ID3D11DeviceContext* d3dContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag)
{
	for (unsigned int i = 0; i < renderComponentIndex; i++)
	{
		auto renderComponent = renderComponents[i];
		auto gameObject = gameObjects[renderComponent.GetGameObjectId()];
		renderComponent.Draw(d3dContext, viewTransform, projectionTransform, updateLag, gameObject);
	}
}

GameObject& ObjectManager::CreateGameObject(const XMFLOAT3 localPosition, const XMFLOAT3 scale)
{
	if (gameObjectIndex == MAX_GAMEOBJECTS_SIZE)
		throw std::exception("Max GameObjects exceeded!");

	gameObjects[gameObjectIndex].Initialize(gameObjectIndex, localPosition, scale);
	return gameObjects[gameObjectIndex++];
}

RenderComponent& ObjectManager::CreateRenderComponent(const unsigned int gameObjectId, std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
{
	if (renderComponentIndex == MAX_RENDERCOMPONENTS_SIZE)
		throw std::exception("Max RenderComponents exceeded!");

	renderComponents[renderComponentIndex].Initialize(renderComponentIndex, gameObjectId, mesh, vertexShader, pixelShader, texture);
	return renderComponents[renderComponentIndex++];
}

void ObjectManager::DeleteGameObject(const unsigned int gameObjectId)
{
	// 1. Delete RenderComponent
	auto gameObject = gameObjects[gameObjectId];
	auto renderComponentId = gameObject.GetRenderComponentId();
	DeleteRenderComponent(renderComponentId);

	// 2. Delete GameObject
	auto lastGameObjectId = --gameObjectIndex;
	memcpy(&gameObjects[gameObjectId], &gameObjects[lastGameObjectId], sizeof(GameObject));

	auto movedGameObject = gameObjects[gameObjectId];
	movedGameObject.SetId(gameObjectId);
	renderComponents[movedGameObject.GetRenderComponentId()].SetGameObjectId(gameObjectId);
}

void ObjectManager::DeleteRenderComponent(const unsigned int renderComponentId)
{
	auto lastRenderComponentId = --renderComponentIndex;
	memcpy(&renderComponents[renderComponentId], &renderComponents[lastRenderComponentId], sizeof(RenderComponent));

	auto movedRenderComponent = renderComponents[renderComponentId];
	movedRenderComponent.SetId(renderComponentId);
	auto gameObjectIdOfMovedRenderComponent = movedRenderComponent.GetGameObjectId();
	gameObjects[gameObjectIdOfMovedRenderComponent].SetRenderComponentId(renderComponentId);
}