#pragma once

#include "Event.h"
#include "../../Layer.h"
#include <DirectXMath.h>

using namespace DirectX;

class EnterWorldSuccessEvent : public Event
{
public:
	EnterWorldSuccessEvent(const int characterId, const XMFLOAT3 position, const int modelId, const int textureId)
		: Event(EventType::EnterWorldSuccess),
		  characterId{ characterId },
		  position{ position },
		  modelId{ modelId },
		  textureId{ textureId }
	{
	}
	const int characterId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;
};