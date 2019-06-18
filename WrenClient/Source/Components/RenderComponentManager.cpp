#include "stdafx.h"
#include "RenderComponentManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;

RenderComponentManager::RenderComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

void RenderComponentManager::Render(ID3D11DeviceContext* d3dContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag)
{
	for (unsigned int i = 0; i < renderComponentIndex; i++)
	{
		auto renderComponent = renderComponents[i];
		auto gameObject = objectManager.GetGameObjectById(renderComponent.gameObjectId);
		renderComponent.Draw(d3dContext, viewTransform, projectionTransform, updateLag, gameObject);
	}
}

RenderComponent& RenderComponentManager::CreateRenderComponent(const long gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
{
	if (renderComponentIndex == MAX_RENDERCOMPONENTS_SIZE)
		throw std::exception("Max RenderComponents exceeded!");

	renderComponents[renderComponentIndex].Initialize(renderComponentIndex, gameObjectId, mesh, vertexShader, pixelShader, texture);
	idIndexMap[renderComponentIndex] = renderComponentIndex;
	return renderComponents[renderComponentIndex++];
}

void RenderComponentManager::DeleteRenderComponent(const unsigned int renderComponentId)
{
	// first copy the last RenderComponent into the index that was deleted
	auto renderComponentToDeleteIndex = idIndexMap[renderComponentId];
	auto lastRenderComponentIndex = --renderComponentIndex;
	memcpy(&renderComponents[renderComponentToDeleteIndex], &renderComponents[lastRenderComponentIndex], sizeof(RenderComponent));

	// then update the index of the moved RenderComponent
	auto lastRenderComponentId = renderComponents[renderComponentToDeleteIndex].id;
	idIndexMap[lastRenderComponentId] = renderComponentToDeleteIndex;
}

RenderComponent& RenderComponentManager::GetRenderComponentById(const unsigned int renderComponentId)
{
	const auto index = idIndexMap[renderComponentId];
	return renderComponents[index];
}

const bool RenderComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;
			const auto gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteRenderComponent(gameObject.renderComponentId);
			break;
		}
	}
	return false;
}

RenderComponentManager::~RenderComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}