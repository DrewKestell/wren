#pragma once

#include "CardinalDirection.h"

class PlayerUpdate
{
	const int m_id;
	const XMFLOAT3 m_position;
	const std::unique_ptr<const std::string> m_state;
	const CardinalDirection m_movementDirection;
	const float m_deltaTime;
public:
	PlayerUpdate(
		const int id,
		const XMFLOAT3 position,
		std::unique_ptr<std::string> state,
		const CardinalDirection movementDirection,
		const float deltaTime);
};