#include "stdafx.h"
#include "PlayerUpdate.h"

PlayerUpdate::PlayerUpdate(
	const int id,
	const XMFLOAT3 position,
	const XMFLOAT3 m_movementVector,
	const float deltaTime)
	: m_id{ id },
	  m_position{ position },
	  m_movementVector{ m_movementVector },
	  m_deltaTime{ deltaTime }
{
}