#pragma once

#include "Event.h"

class GameObjectUpdateEvent : public Event
{
public:
	GameObjectUpdateEvent(const int characterId, const float posX, const float posY, const float posZ, const float movX, const float movY, const float movZ)
		: Event(EventType::GameObjectUpdate),
		  characterId{ characterId },
		  posX{ posX },
		  posY{ posY },
		  posZ{ posZ },
		  movX{ movX },
		  movY{ movY },
		  movZ{ movZ }
	{
	}
	const int characterId{ 0 };
	const float posX{ 0.0f };
	const float posY{ 0.0f };
	const float posZ{ 0.0f };
	const float movX{ 0.0f };
	const float movY{ 0.0f };
	const float movZ{ 0.0f };
};