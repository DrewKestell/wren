#include "stdafx.h"
#include "RenderComponentManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

RenderComponentManager::RenderComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager }
{
	eventHandler.Subscribe(*this);
}

void RenderComponentManager::Render(ID3D11DeviceContext* d3dContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag)
{
	for (auto i = 0; i < renderComponentIndex; i++)
	{
		RenderComponent& renderComponent{ renderComponents[i] };
		GameObject& gameObject{ objectManager.GetGameObjectById(renderComponent.gameObjectId) };
		renderComponent.Draw(d3dContext, viewTransform, projectionTransform, updateLag, gameObject);
	}
}

RenderComponent& RenderComponentManager::CreateRenderComponent(const int gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
{
	if (renderComponentIndex == MAX_RENDERCOMPONENTS_SIZE)
		throw std::exception("Max RenderComponents exceeded!");

	renderComponents[renderComponentIndex].Initialize(renderComponentIndex, gameObjectId, mesh, vertexShader, pixelShader, texture);
	idIndexMap[renderComponentIndex] = renderComponentIndex;
	return renderComponents[renderComponentIndex++];
}

void RenderComponentManager::DeleteRenderComponent(const int renderComponentId)
{
	// first copy the last RenderComponent into the index that was deleted
	const auto renderComponentToDeleteIndex = idIndexMap[renderComponentId];
	const auto lastRenderComponentIndex = --renderComponentIndex;
	memcpy(&renderComponents[renderComponentToDeleteIndex], &renderComponents[lastRenderComponentIndex], sizeof(RenderComponent));

	// then update the index of the moved RenderComponent
	const auto lastRenderComponentId = renderComponents[renderComponentToDeleteIndex].id;
	idIndexMap[lastRenderComponentId] = renderComponentToDeleteIndex;
}

RenderComponent& RenderComponentManager::GetRenderComponentById(const int renderComponentId)
{
	const auto index = idIndexMap[renderComponentId];
	return renderComponents[index];
}

const bool RenderComponentManager::HandleEvent(const Event* const event)
{
	const auto type{ event->type };
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;
			const GameObject& gameObject{ objectManager.GetGameObjectById(derivedEvent->gameObjectId) };
			DeleteRenderComponent(gameObject.renderComponentId);
			break;
		}
	}
	return false;
}

RenderComponentManager::~RenderComponentManager()
{
	eventHandler.Unsubscribe(*this);
}