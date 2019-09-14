#include "stdafx.h"
#include <OpCodes.h>
#include <Utility.h>
#include "SkillComponentManager.h"
#include "PlayerComponentManager.h"
#include "../SocketManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Events/DeleteGameObjectEvent.h"
#include "../Events/AttackHitEvent.h"
#include "../Events/AttackMissEvent.h"
#include "../Models/Skill.h"

extern EventHandler g_eventHandler;
extern SocketManager g_socketManager;
extern PlayerComponentManager g_playerComponentManager;

SkillComponentManager::SkillComponentManager(ObjectManager& objectManager)
	: objectManager{ objectManager }
{
	g_eventHandler.Subscribe(*this);
}

SkillComponent& SkillComponentManager::CreateSkillComponent(const int gameObjectId, std::vector<WrenCommon::Skill>& skills)
{
	if (skillComponentIndex == MAX_SKILLCOMPONENTS_SIZE)
		throw std::exception("Max SkillComponents exceeded!");

	skillComponents[skillComponentIndex].Initialize(skillComponentIndex, gameObjectId, skills);
	idIndexMap[skillComponentIndex] = skillComponentIndex;
	return skillComponents[skillComponentIndex++];
}

void SkillComponentManager::DeleteSkillComponent(const int skillComponentId)
{
	// first copy the last SkillComponent into the index that was deleted
	auto skillComponentToDeleteIndex = idIndexMap[skillComponentId];
	auto lastSkillComponentIndex = --skillComponentIndex;
	memcpy(&skillComponents[skillComponentToDeleteIndex], &skillComponents[lastSkillComponentIndex], sizeof(SkillComponent));

	// then update the index of the moved SkillComponent
	auto lastSkillComponentId = skillComponents[skillComponentToDeleteIndex].id;
	idIndexMap[lastSkillComponentId] = skillComponentToDeleteIndex;
}

SkillComponent& SkillComponentManager::GetSkillComponentById(const int skillComponentId)
{
	const auto index = idIndexMap[skillComponentId];
	return skillComponents[index];
}

const bool SkillComponentManager::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;

			const auto gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteSkillComponent(gameObject.skillComponentId);

			break;
		}
		case EventType::AttackHit:
		{
			const auto derivedEvent = (AttackHitEvent*)event;

			// TODO: handle fancy logic based on NPC's DifficultyRating, etc

			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> dist100(0, 99);

			// first try to increase the attacker's skills
			const auto attacker = objectManager.GetGameObjectById(derivedEvent->attackerId);

			if (attacker.skillComponentId >= 0) // this is initialized as -1, so we check if this object has a skillComponent (NPCs don't for now)
			{
				const auto attackerSkillComp = GetSkillComponentById(attacker.skillComponentId);
				const auto attackerPlayerComp = g_playerComponentManager.GetPlayerComponentById(attacker.playerComponentId);

				for (auto i = 0; i < derivedEvent->weaponSkillArrLen; i++)
				{
					const auto weaponSkillId = derivedEvent->weaponSkillIds[i];
					WrenServer::Skill* weaponSkill = attackerSkillComp.skills[weaponSkillId];

					// 100 is the max skill level, so we skip this weapon skill in that case
					if (weaponSkill->value < 100)
					{
						const auto roll = dist100(rng);
						if (roll > 50)
						{
							weaponSkill->value++;

							std::string args[]{ std::to_string(weaponSkillId), std::to_string(weaponSkill->value) };
							g_socketManager.SendPacket(attackerPlayerComp.fromSockAddr, OpCode::SkillIncrease, args, 2);
						}
					}
				}
			}

			// next try to increase the defender's skills
			const auto target = objectManager.GetGameObjectById(derivedEvent->targetId);
			
			if (target.skillComponentId >= 0) // this is initialized as -1, so we check if this object has a skillComponent (NPCs don't for now)
			{
				const auto targetSkillComp = GetSkillComponentById(target.skillComponentId);
				const auto targetPlayerComp = g_playerComponentManager.GetPlayerComponentById(target.playerComponentId);

				const auto defenseSkillId = 2;
				WrenServer::Skill* defenseSkill = targetSkillComp.skills[defenseSkillId];

				// 100 is the max skill level, so we skip this weapon skill in that case
				if (defenseSkill->value < 100)
				{
					const auto roll = dist100(rng);
					if (roll > 50)
					{
						defenseSkill->value++;

						std::string args[]{ std::to_string(defenseSkillId), std::to_string(defenseSkill->value) };
						g_socketManager.SendPacket(targetPlayerComp.fromSockAddr, OpCode::SkillIncrease, args, 2);
					}
				}
			}

			break;
		}
		case EventType::AttackMiss:
		{
			const auto derivedEvent = (AttackMissEvent*)event;

			const auto attacker = objectManager.GetGameObjectById(derivedEvent->attackerId);
			const auto attackerSkillComp = GetSkillComponentById(attacker.skillComponentId);

			const auto target = objectManager.GetGameObjectById(derivedEvent->targetId);
			const auto targetSkillComp = GetSkillComponentById(attacker.skillComponentId);

			break;
		}
	}
	return false;
}

void SkillComponentManager::Update()
{
	
}

SkillComponentManager::~SkillComponentManager()
{
	g_eventHandler.Unsubscribe(*this);
}