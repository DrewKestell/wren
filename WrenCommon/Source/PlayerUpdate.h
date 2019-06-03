#pragma once

class PlayerUpdate
{
public:
	PlayerUpdate(
		const int id,
		const XMFLOAT3 position,
		const bool isRightClickHeld,
		const XMFLOAT3 currentMouseDirection);

	const int id;
	const XMFLOAT3 position;
	const bool isRightClickHeld;
	const XMFLOAT3 currentMouseDirection;
};