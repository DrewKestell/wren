#include "stdafx.h"
#include "PlayerUpdate.h"

PlayerUpdate::PlayerUpdate(
	const int id,
	const XMFLOAT3 position,
	const XMFLOAT3 movementVector,
	const float deltaTime)
	: id{ id },
	  position{ position },
	  movementVector{ movementVector },
	  deltaTime{ deltaTime }
{
}