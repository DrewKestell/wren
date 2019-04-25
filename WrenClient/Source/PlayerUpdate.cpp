#include "stdafx.h"
#include "PlayerUpdate.h"

PlayerUpdate::PlayerUpdate(
	const int id,
	const XMFLOAT3 position,
	std::unique_ptr<std::string> state,
	const CardinalDirection movementDirection,
	const float deltaTime)
	: m_id{ id },
	  m_position{ position },
	  m_state{ std::move(state) },
	  m_movementDirection{ movementDirection },
	  m_deltaTime{ deltaTime }
{
}