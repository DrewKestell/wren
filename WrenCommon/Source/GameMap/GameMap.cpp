#include "stdafx.h"
#include "GameMap.h"

GameMapTile& GameMap::GetTileByPos(const XMFLOAT3 pos)
{
	int row, col;
	Utility::GetMapTileXYFromPos(pos, row, col);

	return mapTiles[(row * MAP_WIDTH) + col];
}

const bool GameMap::IsTileOccupied(const XMFLOAT3 pos)
{
	return GetTileByPos(pos).isOccupied;
}

const void GameMap::SetTileOccupied(const XMFLOAT3 pos, const bool isOccupied)
{
	GetTileByPos(pos).isOccupied = isOccupied;
}