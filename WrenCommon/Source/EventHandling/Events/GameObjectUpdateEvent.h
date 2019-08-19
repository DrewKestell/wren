#pragma once

#include "Event.h"
#include <GameObjectType.h>

class GameObjectUpdateEvent : public Event
{
public:
	GameObjectUpdateEvent(
		const int gameObjectId,
		const float posX, const float posY, const float posZ,
		const float movX, const float movY, const float movZ,
		const GameObjectType type)
		: Event(EventType::GameObjectUpdate),
		  gameObjectId{ gameObjectId },
		  posX{ posX }, posY{ posY }, posZ{ posZ },
		  movX{ movX }, movY{ movY }, movZ{ movZ },
		  type{ type }
	{
	}
	const int gameObjectId;
	const float posX;
	const float posY;
	const float posZ;
	const float movX;
	const float movY;
	const float movZ;
	const GameObjectType type;
};