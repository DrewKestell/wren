#pragma once

class PlayerUpdate
{
public:
	PlayerUpdate(
		const int id,
		const XMFLOAT3 position,
		const XMFLOAT3 m_movementVector,
		const float deltaTime);

	const int id;
	const XMFLOAT3 position;
	const XMFLOAT3 movementVector;
	const float deltaTime;
};