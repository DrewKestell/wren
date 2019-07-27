#include "stdafx.h"
#include <Utility.h>
#include "PlayerComponentManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;

PlayerComponentManager::PlayerComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

PlayerComponent& PlayerComponentManager::CreatePlayerComponent(const long gameObjectId)
{
	if (playerComponentIndex == MAX_PLAYERCOMPONENTS_SIZE)
		throw std::exception("Max PlayerComponents exceeded!");

	playerComponents[playerComponentIndex].Initialize(playerComponentIndex, gameObjectId);
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
	for (unsigned int i = 0; i < playerComponentIndex; i++)
	{
		PlayerComponent& comp = playerComponents[i];

	}
}

PlayerComponentManager::~PlayerComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}