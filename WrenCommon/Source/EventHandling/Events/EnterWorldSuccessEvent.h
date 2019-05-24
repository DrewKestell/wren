#pragma once

#include <Models/Skill.h>
#include <Models/Ability.h>
#include "Event.h"
#include "../../Layer.h"
#include <DirectXMath.h>

using namespace DirectX;

class EnterWorldSuccessEvent : public Event
{
public:
	EnterWorldSuccessEvent(const int characterId, const XMFLOAT3 position, const int modelId, const int textureId, std::vector<Skill*>* skills, std::vector<Ability*>* abilities)
		: Event(EventType::EnterWorldSuccess),
		  characterId{ characterId },
		  position{ position },
		  modelId{ modelId },
		  textureId{ textureId },
		  skills{ skills },
		  abilities{ abilities }
	{
	}
	const int characterId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;
	std::vector<Skill*>* skills;
	std::vector<Ability*>* abilities;
};