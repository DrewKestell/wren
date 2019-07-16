#include "stdafx.h"
#include "AIComponentManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"
#include <Constants.h>
#include <Utility.h>

extern EventHandler g_eventHandler;

AIComponentManager::AIComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

AIComponent& AIComponentManager::CreateAIComponent(const long gameObjectId)
{
	if (aiComponentIndex == MAX_AICOMPONENTS_SIZE)
		throw std::exception("Max RenderComponents exceeded!");

	aiComponents[aiComponentIndex].Initialize(aiComponentIndex, gameObjectId);
	idIndexMap[aiComponentIndex] = aiComponentIndex;
	return aiComponents[aiComponentIndex++];
}

void AIComponentManager::DeleteAIComponent(const unsigned int aiComponentId)
{
	// first copy the last RenderComponent into the index that was deleted
	auto aiComponentToDeleteIndex = idIndexMap[aiComponentId];
	auto lastAIComponentIndex = --aiComponentIndex;
	memcpy(&aiComponents[aiComponentToDeleteIndex], &aiComponents[lastAIComponentIndex], sizeof(AIComponent));

	// then update the index of the moved RenderComponent
	auto lastAIComponentId = aiComponents[aiComponentToDeleteIndex].id;
	idIndexMap[lastAIComponentId] = aiComponentToDeleteIndex;
}

AIComponent& AIComponentManager::GetAIComponentById(const unsigned int aiComponentId)
{
	const auto index = idIndexMap[aiComponentId];
	return aiComponents[index];
}

const bool AIComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
	case EventType::DeleteGameObject:
	{
		const auto derivedEvent = (DeleteGameObjectEvent*)event;
		const auto gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
		DeleteAIComponent(gameObject.aiComponentId);
		break;
	}
	}
	return false;
}

void AIComponentManager::Update()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist100(0, 99);
	std::uniform_int_distribution<std::mt19937::result_type> dist8(0, 7);

	for (unsigned int i = 0; i < aiComponentIndex; i++)
	{
		auto comp = aiComponents[i];
		int rnd;
		if (!comp.isMoving)
		{
			rnd = dist100(rng);
			if (rnd == 0)
			{
				comp.isMoving = true;

				rnd = dist8(rng);

				if (rnd == 0)
					comp.movementVector = XMFLOAT3{ -1.0f, 0.0f, -1.0f };
				else if (rnd == 1)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else if (rnd == 2)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else if (rnd == 3)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else if (rnd == 4)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else if (rnd == 5)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else if (rnd == 6)
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };
				else
					comp.movementVector = XMFLOAT3{ 0.0f, 0.0f, -1.0f };

				auto gameObject = objectManager.GetGameObjectById(comp.gameObjectId);
				auto pos = gameObject.GetWorldPosition();
				comp.destination = XMFLOAT3
				{
					pos.x + (comp.movementVector.x * TILE_SIZE),
					pos.y + (comp.movementVector.y * TILE_SIZE),
					pos.z + (comp.movementVector.z * TILE_SIZE)
				};
			}
		}

		if (comp.isMoving)
		{

		}
	}
}

AIComponentManager::~AIComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}