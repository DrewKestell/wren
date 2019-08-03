#include "stdafx.h"
#include <OpCodes.h>
#include <Utility.h>
#include "PlayerComponentManager.h"
#include "AIComponentManager.h"
#include "../SocketManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;
extern AIComponentManager g_aiComponentManager;
extern SocketManager g_socketManager;

PlayerComponentManager::PlayerComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

PlayerComponent& PlayerComponentManager::CreatePlayerComponent(const long gameObjectId, const std::string token, const std::string ipAndPort, const sockaddr_in fromSockAddr, const DWORD lastHeartbeat)
{
	if (playerComponentIndex == MAX_PLAYERCOMPONENTS_SIZE)
		throw std::exception("Max PlayerComponents exceeded!");

	playerComponents[playerComponentIndex].Initialize(playerComponentIndex, gameObjectId, token, ipAndPort, fromSockAddr, lastHeartbeat);
	idIndexMap[playerComponentIndex] = playerComponentIndex;
	return playerComponents[playerComponentIndex++];
}

void PlayerComponentManager::DeletePlayerComponent(const unsigned int playerComponentId)
{
	// first copy the last AIComponent into the index that was deleted
	auto playerComponentToDeleteIndex = idIndexMap[playerComponentId];
	auto lastPlayerComponentIndex = --playerComponentIndex;
	memcpy(&playerComponents[playerComponentToDeleteIndex], &playerComponents[lastPlayerComponentIndex], sizeof(PlayerComponent));

	// then update the index of the moved AIComponent
	auto lastAIComponentId = playerComponents[playerComponentToDeleteIndex].id;
	idIndexMap[lastAIComponentId] = playerComponentToDeleteIndex;
}

PlayerComponent& PlayerComponentManager::GetPlayerComponentById(const unsigned int playerComponentId)
{
	const auto index = idIndexMap[playerComponentId];
	return playerComponents[index];
}

const bool PlayerComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;
			const auto gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
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

	for (unsigned int i = 0; i < playerComponentIndex; i++)
	{
		PlayerComponent& comp = playerComponents[i];

		const auto weaponSpeed = 3.0f;
		const auto damageMin = 2;
		const auto damageMax = 8;

		if (comp.swingTimer < weaponSpeed)
			comp.swingTimer += UPDATE_FREQUENCY;

		if (comp.autoAttackOn && comp.targetId >= 0)
		{
			const auto player = objectManager.GetGameObjectById(comp.gameObjectId);
			const auto target = objectManager.GetGameObjectById(comp.targetId);

			if (Utility::AreOnAdjacentOrDiagonalTiles(player.localPosition, target.localPosition))
			{
				if (comp.swingTimer >= weaponSpeed)
				{
					comp.swingTimer = 0.0f;

					AIComponent& targetAIComponent = g_aiComponentManager.GetAIComponentById(target.aiComponentId);

					if (targetAIComponent.targetId == -1)
						targetAIComponent.targetId = player.id;

					const auto hit = dist100(rng) > 30;
					if (hit)
					{
						std::uniform_int_distribution<std::mt19937::result_type> distDamage(damageMin, damageMax);

						const auto dmg = distDamage(rng);
						std::string args[]{ std::to_string(player.id), std::to_string(target.id), std::to_string(dmg) };
						g_socketManager.SendPacket(OPCODE_ATTACK_HIT, args, 3);
					}
					else
					{
						std::string args[]{ std::to_string(player.id), std::to_string(target.id) };
						g_socketManager.SendPacket(OPCODE_ATTACK_MISS, args, 2);
					}
				}
			}
		}
	}
}

PlayerComponent* PlayerComponentManager::GetPlayerComponents()
{
	return playerComponents;
}

const unsigned int PlayerComponentManager::GetPlayerComponentIndex()
{
	return playerComponentIndex;
}

PlayerComponentManager::~PlayerComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}