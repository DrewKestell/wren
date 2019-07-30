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
	EnterWorldSuccessEvent(const int accountId, const XMFLOAT3 position, const int modelId, const int textureId, std::vector<Skill*>* skills, std::vector<Ability*>* abilities, std::string* name)
		: Event(EventType::EnterWorldSuccess),
		  accountId{ accountId },
		  position{ position },
		  modelId{ modelId },
		  textureId{ textureId },
		  skills{ skills },
		  abilities{ abilities },
		  name{ name }
	{
	}
	const int accountId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;
	std::vector<Skill*>* skills;
	std::vector<Ability*>* abilities;
	std::string* name;
};