#pragma once

#include "CardinalDirection.h"

class PlayerUpdate
{
	const int m_id;
	const XMFLOAT3 m_position;
	const XMFLOAT3 m_movementVector;
	const float m_deltaTime;
public:
	PlayerUpdate(
		const int id,
		const XMFLOAT3 position,
		const XMFLOAT3 m_movementVector,
		const float deltaTime);
};