#include "stdafx.h"
#include "StatsComponent.h"

void StatsComponent::Initialize(const unsigned int id, const long gameObjectId, const int health, const int maxHealth, const int mana, const int maxMana, const int stamina, const int maxStamina, const int agility, const int strength, const int wisdom, const int intelligence, const int charisma, const int luck, const int endurance)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
	this->health = health;
	this->maxHealth = maxHealth;
	this->mana = mana;
	this->maxMana = maxMana;
	this->stamina = stamina;
	this->maxStamina = maxStamina;
	this->agility = agility;
	this->strength = strength;
	this->wisdom = wisdom;
	this->intelligence = intelligence;
	this->charisma = charisma;
	this->luck = luck;
	this->endurance = endurance;
}