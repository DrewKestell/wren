#include "stdafx.h"
#include "SkillComponentManager.h"
#include "PlayerComponentManager.h"
#include "../Events/AttackHitEvent.h"
#include "../Events/AttackMissEvent.h"

SkillComponentManager::SkillComponentManager(EventHandler& eventHandler, ObjectManager& objectManager, ServerComponentOrchestrator& componentOrchestrator, ServerSocketManager& socketManager)
	: ComponentManager(eventHandler, objectManager),
	  componentOrchestrator{ componentOrchestrator },
	  socketManager{ socketManager }
{
}

SkillComponent& SkillComponentManager::CreateSkillComponent(const int gameObjectId, std::vector<WrenCommon::Skill>& skills)
{
	SkillComponent& skillComponent = CreateComponent(gameObjectId);

	for (auto i = 0; i < skills.size(); i++)
	{
		const auto skill = skills.at(i);
		skillComponent.skills.push_back(std::make_unique<WrenServer::Skill>(skill.skillId, skill.value));
	}

	return skillComponent;
}

const bool SkillComponentManager::HandleEvent(const Event* const event)
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
		case EventType::AttackHit:
		{
			const auto derivedEvent = (AttackHitEvent*)event;

			// TODO: handle fancy logic based on NPC's DifficultyRating, etc

			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> dist100(0, 99);

			const auto playerComponentManager = componentOrchestrator.GetPlayerComponentManager();

			// first try to increase the attacker's skills
			const GameObject& attacker = objectManager.GetGameObjectById(derivedEvent->attackerId);

			if (attacker.skillComponentId >= 0) // this is initialized as -1, so we check if this object has a skillComponent (NPCs don't for now)
			{
				const SkillComponent& attackerSkillComp = GetComponentById(attacker.skillComponentId);
				const PlayerComponent& attackerPlayerComp = playerComponentManager->GetComponentById(attacker.playerComponentId);

				for (auto i = 0; i < derivedEvent->weaponSkillArrLen; i++)
				{
					const auto weaponSkillId = derivedEvent->weaponSkillIds[i];
					auto weaponSkill = attackerSkillComp.skills[weaponSkillId - 1].get();

					// 100 is the max skill level, so we skip this weapon skill in that case
					if (weaponSkill->value < 100)
					{
						const auto roll = dist100(rng);
						if (roll > 50)
						{
							weaponSkill->value++;

							std::vector<std::string> args{ std::to_string(weaponSkillId), std::to_string(weaponSkill->value) };
							socketManager.SendPacket(attackerPlayerComp.GetFromSockAddr(), OpCode::SkillIncrease, args);
						}
					}
				}
			}

			// next try to increase the defender's skills
			const GameObject& target = objectManager.GetGameObjectById(derivedEvent->targetId);
			
			if (target.skillComponentId >= 0) // this is initialized as -1, so we check if this object has a skillComponent (NPCs don't for now)
			{
				const SkillComponent& targetSkillComp = GetComponentById(target.skillComponentId);
				const PlayerComponent& targetPlayerComp = playerComponentManager->GetComponentById(target.playerComponentId);

				const auto defenseSkillId = 2;
				WrenServer::Skill* defenseSkill = targetSkillComp.skills[defenseSkillId - 1].get();

				// 100 is the max skill level, so we skip this weapon skill in that case
				if (defenseSkill->value < 100)
				{
					const auto roll = dist100(rng);
					if (roll > 50)
					{
						defenseSkill->value++;

						std::vector<std::string> args{ std::to_string(defenseSkillId), std::to_string(defenseSkill->value) };
						socketManager.SendPacket(targetPlayerComp.GetFromSockAddr(), OpCode::SkillIncrease, args);
					}
				}
			}

			break;
		}
		case EventType::AttackMiss:
		{
			const auto derivedEvent = (AttackMissEvent*)event;

			const GameObject& attacker = objectManager.GetGameObjectById(derivedEvent->attackerId);
			const SkillComponent& attackerSkillComp = GetComponentById(attacker.skillComponentId);

			const GameObject& target = objectManager.GetGameObjectById(derivedEvent->targetId);
			const SkillComponent& targetSkillComp = GetComponentById(attacker.skillComponentId);

			break;
		}
	}
	return false;
}

void SkillComponentManager::Update()
{
	
}