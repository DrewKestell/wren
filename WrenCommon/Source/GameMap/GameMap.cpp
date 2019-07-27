#include "stdafx.h"
#include "GameMap.h"

GameMapTile& GameMap::GetTileByPos(const XMFLOAT3 pos)
{
	const auto row = (int)pos.x / (int)TILE_SIZE;
	const auto col = (int)pos.z / (int)TILE_SIZE;

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