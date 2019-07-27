#include "stdafx.h"
#include <Utility.h>
#include "AIComponentManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;

static constexpr XMFLOAT3 DIRECTIONS[8]
{
	VEC_SOUTHWEST,
	VEC_SOUTH,
	VEC_SOUTHEAST,
	VEC_EAST,
	VEC_NORTHEAST,
	VEC_NORTH,
	VEC_NORTHWEST,
	VEC_WEST
};

AIComponentManager::AIComponentManager(ObjectManager& objectManager, GameMap& gameMap)
	: objectManager{ objectManager },
	  gameMap{ gameMap }
{
	g_eventHandler.Subscribe(*this);
}

AIComponent& AIComponentManager::CreateAIComponent(const long gameObjectId)
{
	if (aiComponentIndex == MAX_AICOMPONENTS_SIZE)
		throw std::exception("Max AIComponents exceeded!");

	aiComponents[aiComponentIndex].Initialize(aiComponentIndex, gameObjectId);
	idIndexMap[aiComponentIndex] = aiComponentIndex;
	return aiComponents[aiComponentIndex++];
}

void AIComponentManager::DeleteAIComponent(const unsigned int aiComponentId)
{
	// first copy the last AIComponent into the index that was deleted
	auto aiComponentToDeleteIndex = idIndexMap[aiComponentId];
	auto lastAIComponentIndex = --aiComponentIndex;
	memcpy(&aiComponents[aiComponentToDeleteIndex], &aiComponents[lastAIComponentIndex], sizeof(AIComponent));

	// then update the index of the moved AIComponent
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
		AIComponent& comp = aiComponents[i];
		int rnd;

		GameObject& gameObject = objectManager.GetGameObjectById(comp.gameObjectId);
		auto pos = gameObject.GetWorldPosition();
		auto destination = gameObject.destination;

		if (gameObject.isMoving)
		{
			// if target is reached
			auto deltaX = std::abs(pos.x - destination.x);
			auto deltaZ = std::abs(pos.z - destination.z);

			if (deltaX < 1.0f && deltaZ < 1.0f)
			{
				gameObject.localPosition = XMFLOAT3{ destination.x, 0.0f, destination.z };
				gameObject.movementVector = VEC_ZERO;
				gameObject.isMoving = false;
			}
		}
		
		if (!gameObject.isMoving)
		{
			rnd = dist100(rng);
			if (rnd == 0)
			{
				rnd = dist8(rng);

				const auto vec = DIRECTIONS[rnd];
				const auto movementVector = XMFLOAT3{ vec.x * TILE_SIZE, vec.y * TILE_SIZE, vec.z * TILE_SIZE };
				const auto proposedPos = Utility::XMFLOAT3Sum(gameObject.localPosition, movementVector);

				if (Utility::CheckOutOfBounds(proposedPos) || gameMap.IsTileOccupied(proposedPos))
					return;

				gameObject.isMoving = true;

				gameObject.movementVector = vec;

				gameObject.destination = XMFLOAT3
				{
					pos.x + (gameObject.movementVector.x * TILE_SIZE),
					pos.y + (gameObject.movementVector.y * TILE_SIZE),
					pos.z + (gameObject.movementVector.z * TILE_SIZE)
				};

				gameMap.SetTileOccupied(gameObject.localPosition, false);
				gameMap.SetTileOccupied(proposedPos, true);
			}
		}
	}
}

AIComponentManager::~AIComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}