#pragma once

#include <Constants.h>
#include "GameMapTile.h"

class GameMap
{
	std::vector<GameMapTile> mapTiles{ MAP_SIZE, GameMapTile{ TerrainType::Dirt } };
	GameMapTile& GetTileByPos(const XMFLOAT3 pos);
public:
	const bool IsTileOccupied(const XMFLOAT3 pos);
	const void SetTileOccupied(const XMFLOAT3 pos, const bool isOccupied);
};