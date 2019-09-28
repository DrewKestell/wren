#include "stdafx.h"
#include "PlayerComponentManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"
#include "../Events/AttackHitEvent.h"
#include "../Events/AttackMissEvent.h"
#include "AIComponentManager.h"
#include <Components/StatsComponentManager.h>

PlayerComponentManager::PlayerComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, GameMap& gameMap, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager },
	  gameMap{ gameMap },
	  componentOrchestrator{ componentOrchestrator },
	  socketManager{ socketManager }
{
	eventHandler.Subscribe(*this);
}

PlayerComponent& PlayerComponentManager::CreatePlayerComponent(const int gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const unsigned __int64 lastHeartbeat)
{
	if (playerComponentIndex == MAX_PLAYERCOMPONENTS_SIZE)
		throw std::exception("Max PlayerComponents exceeded!");

	playerComponents[playerComponentIndex].Initialize(playerComponentIndex, gameObjectId, token, ipAndPort, fromSockAddr, lastHeartbeat);
	idIndexMap[playerComponentIndex] = playerComponentIndex;
	return playerComponents[playerComponentIndex++];
}

void PlayerComponentManager::DeletePlayerComponent(const int playerComponentId)
{
	// first copy the last AIComponent into the index that was deleted
	const auto playerComponentToDeleteIndex = idIndexMap[playerComponentId];
	const auto lastPlayerComponentIndex = --playerComponentIndex;
	memcpy(&playerComponents[playerComponentToDeleteIndex], &playerComponents[lastPlayerComponentIndex], sizeof(PlayerComponent));

	// then update the index of the moved AIComponent
	const auto lastAIComponentId = playerComponents[playerComponentToDeleteIndex].id;
	idIndexMap[lastAIComponentId] = playerComponentToDeleteIndex;
}

PlayerComponent& PlayerComponentManager::GetPlayerComponentById(const int playerComponentId)
{
	const auto index = idIndexMap[playerComponentId];
	return playerComponents[index];
}

const bool PlayerComponentManager::HandleEvent(const Event* const event)
{
	const auto type{ event->type };
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;
			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeletePlayerComponent(gameObject.playerComponentId);
			break;
		}
	}
	return false;
}

void PlayerComponentManager::Update()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist100(0, 99);

	const auto aiComponentManager = componentOrchestrator.GetAIComponentManager();
	const auto statsComponentManager = componentOrchestrator.GetStatsComponentManager();

	for (auto i = 0; i < playerComponentIndex; i++)
	{
		PlayerComponent& comp = playerComponents[i];
		GameObject& player = objectManager.GetGameObjectById(comp.gameObjectId);
		
		// first handle movement
		if (player.movementVector == VEC_ZERO && comp.rightMouseDownDir != VEC_ZERO)
		{
			const auto delta = XMFLOAT3{ comp.rightMouseDownDir.x * TILE_SIZE, comp.rightMouseDownDir.y * TILE_SIZE, comp.rightMouseDownDir.z * TILE_SIZE };
			const auto proposedPos = player.localPosition + delta;

			if (!Utility::CheckOutOfBounds(proposedPos) && !gameMap.IsTileOccupied(proposedPos))
			{
				player.movementVector = comp.rightMouseDownDir;
				player.destination = proposedPos;

				gameMap.SetTileOccupied(player.localPosition, false);
				gameMap.SetTileOccupied(player.destination, true);
			}
		}

		// next handle combat
		// if target dies, toggle off auto attack
		const GameObject& target = objectManager.GetGameObjectById(comp.targetId);
		const StatsComponent& targetStatsComponent = statsComponentManager->GetStatsComponentById(target.statsComponentId);

		if (comp.autoAttackOn && !targetStatsComponent.alive)
		{
			comp.autoAttackOn = false;

			std::vector<std::string> args{ "1" };
			socketManager.SendPacket(comp.GetFromSockAddr(), OpCode::ActivateAbilitySuccess, args);
		}

		const auto weaponSpeed = 3.0f;
		const auto damageMin = 4;
		const auto damageMax = 30;

		if (comp.swingTimer < weaponSpeed)
			comp.swingTimer += UPDATE_FREQUENCY;

		if (comp.autoAttackOn && comp.targetId >= 0 && comp.swingTimer >= weaponSpeed
			&& Utility::AreOnAdjacentOrDiagonalTiles(player.localPosition, target.localPosition))
		{
			comp.swingTimer = 0.0f;

			AIComponent& targetAIComponent = aiComponentManager->GetAIComponentById(target.aiComponentId);

			const auto playerId = player.GetId();
			const auto targetId = target.GetId();

			if (targetAIComponent.targetId == -1)
				targetAIComponent.targetId = playerId;

			const auto hit = dist100(rng) > 30;
			if (hit)
			{
				std::uniform_int_distribution<std::mt19937::result_type> distDamage(damageMin, damageMax);
				const auto dmg = distDamage(rng);

				StatsComponent& statsComponent = statsComponentManager->GetStatsComponentById(target.statsComponentId);
				statsComponent.health = Utility::Max(0, statsComponent.health - dmg);

				const int* const weaponSkillIds = new int[2]{ 1, 2 }; // Hand-to-Hand Combat, Melee
				std::unique_ptr<Event> e = std::make_unique<AttackHitEvent>(playerId, targetId, (int)dmg, weaponSkillIds, 2);
				eventHandler.QueueEvent(e);

				std::vector<std::string> args{ std::to_string(playerId), std::to_string(targetId), std::to_string(dmg) };
				socketManager.SendPacketToAllClients(OpCode::AttackHit, args);
			}
			else
			{
				const int* const weaponSkillIds = new int[2]{ 1, 2 }; // Hand-to-Hand Combat, Melee
				std::unique_ptr<Event> e = std::make_unique<AttackMissEvent>(playerId, targetId, weaponSkillIds, 2);
				eventHandler.QueueEvent(e);

				std::vector<std::string> args{ std::to_string(playerId), std::to_string(targetId) };
				socketManager.SendPacketToAllClients(OpCode::AttackMiss, args);
			}
		}
	}
}

const XMFLOAT3 PlayerComponentManager::GetDestinationVector(const XMFLOAT3 rightMouseDownDir, const XMFLOAT3 playerPos) const
{
	const auto vec = XMLoadFloat3(&rightMouseDownDir);
	const auto currentPos = XMLoadFloat3(&playerPos);
	XMFLOAT3 destinationVector;
	XMStoreFloat3(&destinationVector, currentPos + (vec * TILE_SIZE));
	return destinationVector;
}

PlayerComponent* PlayerComponentManager::GetPlayerComponents()
{
	return playerComponents;
}

const int PlayerComponentManager::GetPlayerComponentIndex()
{
	return playerComponentIndex;
}

PlayerComponentManager::~PlayerComponentManager()
{
	eventHandler.Unsubscribe(*this);
}
