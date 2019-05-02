#pragma once

#include "GameObject.h"
#include "TerrainType.h"

class GameMapTile
{
	TerrainType terrainType;
public:
	GameMapTile(TerrainType terrainType)
		: terrainType{ terrainType }
	{
	}
};