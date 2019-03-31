#pragma once

#include "../GameObject.h"
#include "TerrainType.h"

class GameMapTile : public GameObject
{
	TerrainType terrainType;
public:
	GameMapTile(
		const DirectX::XMFLOAT3 position,
		ObjectManager& objectManager,
		const TerrainType terrainType)
		: GameObject(objectManager, position),
		  terrainType{ terrainType }
	{
	}
	virtual void Draw();
};