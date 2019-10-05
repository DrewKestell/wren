#include "stdafx.h"
#include "StatsComponentManager.h"
#include "EventHandling/Events/NpcDeathEvent.h"

StatsComponentManager::StatsComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: ComponentManager(eventHandler, objectManager)
{
}

StatsComponent& StatsComponentManager::CreateStatsComponent(
	const int gameObjectId,
	const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance,
	const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina)
{
	StatsComponent& statsComponent = CreateComponent(gameObjectId);

	statsComponent.agility = agility;
	statsComponent.strength = strength;
	statsComponent.wisdom = wisdom;
	statsComponent.intelligence = intelligence;
	statsComponent.charisma = charisma;
	statsComponent.luck = luck;
	statsComponent.endurance = endurance;
	statsComponent.health = health;
	statsComponent.maxHealth = maxHealth;
	statsComponent.mana = mana;
	statsComponent.maxMana = maxMana;
	statsComponent.stamina = stamina;
	statsComponent.maxStamina = maxStamina;

	return statsComponent;
}

const bool StatsComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;

			ComponentManager::HandleEvent(event);

			break;
		}
		case EventType::NpcDeath:
		{
			const auto derivedEvent = (NpcDeathEvent*)event;

			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			StatsComponent& statsComponent = GetComponentById(gameObject.statsComponentId);
			statsComponent.alive = false;

			break;
		}
	}
	return false;
}
