#include "stdafx.h"
#include <OpCodes.h>
#include <Utility.h>
#include "AIComponentManager.h"
#include <Components/StatsComponent.h>
#include <Components/StatsComponentManager.h>
#include "../SocketManager.h"
#include "../Events/AttackHitEvent.h"
#include "../Events/AttackMissEvent.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;
extern SocketManager g_socketManager;
extern StatsComponentManager g_statsComponentManager;

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

AIComponent& AIComponentManager::CreateAIComponent(const int gameObjectId)
{
	if (aiComponentIndex == MAX_AICOMPONENTS_SIZE)
		throw std::exception("Max AIComponents exceeded!");

	aiComponents[aiComponentIndex].Initialize(aiComponentIndex, gameObjectId);
	idIndexMap[aiComponentIndex] = aiComponentIndex;
	return aiComponents[aiComponentIndex++];
}

void AIComponentManager::DeleteAIComponent(const int aiComponentId)
{
	// first copy the last AIComponent into the index that was deleted
	auto aiComponentToDeleteIndex = idIndexMap[aiComponentId];
	auto lastAIComponentIndex = --aiComponentIndex;
	memcpy(&aiComponents[aiComponentToDeleteIndex], &aiComponents[lastAIComponentIndex], sizeof(AIComponent));

	// then update the index of the moved AIComponent
	auto lastAIComponentId = aiComponents[aiComponentToDeleteIndex].id;
	idIndexMap[lastAIComponentId] = aiComponentToDeleteIndex;
}

AIComponent& AIComponentManager::GetAIComponentById(const int aiComponentId)
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
		case EventType::AttackHit:
		{
			const auto derivedEvent = (AttackHitEvent*)event;

			
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

	for (auto i = 0; i < aiComponentIndex; i++)
	{
		AIComponent& comp = aiComponents[i];
		GameObject& gameObject = objectManager.GetGameObjectById(comp.gameObjectId);
		auto pos = gameObject.GetWorldPosition();

		// handle movement
		if (gameObject.movementVector == VEC_ZERO)
		{
			auto movementVec = VEC_ZERO;

			if (comp.targetId >= 0)
			{
				const auto target = objectManager.GetGameObjectById(comp.targetId);

				const auto npcPosVec = XMLoadFloat3(&pos);
				const auto targetPosVec = XMLoadFloat3(&target.localPosition);

				float shortestDistance = FLT_MAX;
				for (auto i = 0; i < 8; i++)
				{
					const auto dirVec = XMLoadFloat3(&DIRECTIONS[i]);
					const auto newVec = XMVectorAdd(npcPosVec, dirVec);
					const auto npcToPlayerVec = XMVectorSubtract(newVec, targetPosVec);
					const auto magnitudeVec = XMVector3Length(npcToPlayerVec);

					XMFLOAT3 magnitudeStoredVec;
					XMStoreFloat3(&magnitudeStoredVec, magnitudeVec);
					
					if (std::abs(magnitudeStoredVec.x) < shortestDistance)
					{
						shortestDistance = magnitudeStoredVec.x;
						movementVec = DIRECTIONS[i];
					}
				}
			}
			else
			{
				auto rnd = dist100(rng);
				if (rnd == 0)
				{
					rnd = dist8(rng);
					movementVec = DIRECTIONS[rnd];
				}
			}

			if (movementVec != VEC_ZERO)
			{
				const auto delta = XMFLOAT3{ movementVec.x * TILE_SIZE, movementVec.y * TILE_SIZE, movementVec.z * TILE_SIZE };
				const auto proposedPos = gameObject.localPosition + delta;

				if (!Utility::CheckOutOfBounds(proposedPos) && !gameMap.IsTileOccupied(proposedPos))
				{
					gameObject.movementVector = movementVec;
					gameObject.destination = proposedPos;

					gameMap.SetTileOccupied(gameObject.localPosition, false);
					gameMap.SetTileOccupied(gameObject.destination, true);
				}
			}
		}

		// handle combat
		const auto weaponSpeed = 5.0f;
		const auto damageMin = 1;
		const auto damageMax = 3;

		if (comp.swingTimer < weaponSpeed)
			comp.swingTimer += UPDATE_FREQUENCY;

		if (comp.targetId >= 0)
		{
			const auto target = objectManager.GetGameObjectById(comp.targetId);

			if (Utility::AreOnAdjacentOrDiagonalTiles(gameObject.localPosition, target.localPosition))
			{
				if (comp.swingTimer >= weaponSpeed)
				{
					comp.swingTimer = 0.0f;
					
					const auto gameObjectId = gameObject.GetId();
					const auto targetId = target.GetId();
					const auto hit = dist100(rng) > 50;
					if (hit)
					{
						std::uniform_int_distribution<std::mt19937::result_type> distDamage(damageMin, damageMax);
						const auto dmg = distDamage(rng);

						StatsComponent& statsComponent = g_statsComponentManager.GetStatsComponentById(target.statsComponentId);
						statsComponent.health = Utility::Max(0, statsComponent.health - dmg);

						const int* const weaponSkillIds = new int[2]{ 0, 1 }; // Hand-to-Hand Combat, Melee
						g_eventHandler.QueueEvent(new AttackHitEvent{ gameObjectId, targetId, (int)dmg, weaponSkillIds, 2 });

						std::string args[]{ std::to_string(gameObjectId), std::to_string(targetId), std::to_string(dmg) };
						g_socketManager.SendPacket(OPCODE_ATTACK_HIT, args, 3);
					}
					else
					{
						const int* const weaponSkillIds = new int[2]{ 0, 1 }; // Hand-to-Hand Combat, Melee
						g_eventHandler.QueueEvent(new AttackMissEvent{ gameObjectId, targetId, weaponSkillIds, 2 });

						std::string args[]{ std::to_string(gameObjectId), std::to_string(targetId)};
						g_socketManager.SendPacket(OPCODE_ATTACK_MISS, args, 2);
					}
				}
			}
		}
	}
}

AIComponentManager::~AIComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}