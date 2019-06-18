#include "stdafx.h"
#include "StatsComponentManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"

extern EventHandler g_eventHandler;

StatsComponentManager::StatsComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

StatsComponent& StatsComponentManager::CreateStatsComponent(const long gameObjectId, const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina, const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance)
{
	if (statsComponentIndex == MAX_STATSCOMPONENTS_SIZE)
		throw std::exception("Max StatsComponents exceeded!");

	statsComponents[statsComponentIndex].Initialize(statsComponentIndex, gameObjectId, health, maxHealth, mana, maxMana, stamina, maxStamina, agility, strength, wisdom, intelligence, charisma, luck, endurance);
	idIndexMap[statsComponentIndex] = statsComponentIndex;
	return statsComponents[statsComponentIndex++];
}

void StatsComponentManager::DeleteStatsComponent(const unsigned int statsComponentId)
{
	// first copy the last StatsComponent into the index that was deleted
	auto statsComponentToDeleteIndex = idIndexMap[statsComponentId];
	auto lastStatsComponentIndex = --statsComponentIndex;
	memcpy(&statsComponents[statsComponentToDeleteIndex], &statsComponents[lastStatsComponentIndex], sizeof(StatsComponent));

	// then update the index of the moved StatsComponent
	auto lastStatsComponentId = statsComponents[statsComponentToDeleteIndex].id;
	idIndexMap[lastStatsComponentId] = statsComponentToDeleteIndex;
}

StatsComponent& StatsComponentManager::GetStatsComponentById(const unsigned int statsComponentId)
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
			const auto gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteStatsComponent(gameObject.statsComponentId);
			break;
		}
	}
	return false;
}

StatsComponentManager::~StatsComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}