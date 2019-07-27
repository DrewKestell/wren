#pragma once

#include "GameObject.h"
#include "TerrainType.h"

class GameMapTile
{
	TerrainType terrainType;
public:
	bool isOccupied{ false };

	GameMapTile(TerrainType terrainType)
		: terrainType{ terrainType }
	{
	}
};