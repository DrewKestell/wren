#include "stdafx.h"
#include "PlayerUpdate.h"

PlayerUpdate::PlayerUpdate(
	const int id,
	const XMFLOAT3 position,
	const bool isRightClickHeld,
	const XMFLOAT3 currentMouseDirection)
	: id{ id },
	  position{ position },
	  isRightClickHeld{ isRightClickHeld },
	  currentMouseDirection{ currentMouseDirection }
{
}