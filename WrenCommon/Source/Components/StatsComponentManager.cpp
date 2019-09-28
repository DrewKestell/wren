#include "stdafx.h"
#include "StatsComponentManager.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"
#include "EventHandling/Events/NpcDeathEvent.h"

StatsComponentManager::StatsComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager }	  
{
	eventHandler.Subscribe(*this);
}

StatsComponent& StatsComponentManager::CreateStatsComponent(
	const int gameObjectId,
	const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
	const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina)
{
	if (statsComponentIndex == MAX_STATSCOMPONENTS_SIZE)
		throw std::exception("Max StatsComponents exceeded!");

	statsComponents[statsComponentIndex].Initialize(statsComponentIndex, gameObjectId, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina);
	idIndexMap[statsComponentIndex] = statsComponentIndex;
	return statsComponents[statsComponentIndex++];
}

void StatsComponentManager::DeleteStatsComponent(const int statsComponentId)
{
	// first copy the last StatsComponent into the index that was deleted
	const auto statsComponentToDeleteIndex = idIndexMap[statsComponentId];
	const auto lastStatsComponentIndex = --statsComponentIndex;
	memcpy(&statsComponents[statsComponentToDeleteIndex], &statsComponents[lastStatsComponentIndex], sizeof(StatsComponent));

	// then update the index of the moved StatsComponent
	const auto lastStatsComponentId = statsComponents[statsComponentToDeleteIndex].id;
	idIndexMap[lastStatsComponentId] = statsComponentToDeleteIndex;
}

StatsComponent& StatsComponentManager::GetStatsComponentById(const int statsComponentId)
{
	const auto index = idIndexMap[statsComponentId];
	return statsComponents[index];
}

const bool StatsComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;

			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteStatsComponent(gameObject.statsComponentId);

			break;
		}
		case EventType::NpcDeath:
		{
			const auto derivedEvent = (NpcDeathEvent*)event;

			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			StatsComponent& statsComponent = GetStatsComponentById(gameObject.statsComponentId);
			statsComponent.alive = false;

			break;
		}
	}
	return false;
}

StatsComponentManager::~StatsComponentManager()
{
	eventHandler.Unsubscribe(*this);
}