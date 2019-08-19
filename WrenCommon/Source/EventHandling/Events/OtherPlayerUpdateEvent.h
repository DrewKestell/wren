#pragma once

#include "Event.h"

class OtherPlayerUpdateEvent : public Event
{
public:
	OtherPlayerUpdateEvent(
		const int accountId,
		const float posX, const float posY, const float posZ,
		const float movX, const float movY, const float movZ,
		const int modelId,
		const int textureId,
		std::string* name)
		: Event(EventType::OtherPlayerUpdate),
		  accountId{ accountId },
		  posX{ posX }, posY{ posY }, posZ{ posZ },
		  movX{ movX }, movY{ movY }, movZ{ movZ },
		  modelId{ modelId },
		  textureId{ textureId },
		  name{ name }
	{
	}
	const int accountId;
	const float posX;
	const float posY;
	const float posZ;
	const float movX;
	const float movY;
	const float movZ;
	const int modelId;
	const int textureId;
	std::string* name;
};