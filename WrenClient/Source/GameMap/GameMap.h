#pragma once

#include <vector>
#include "../ObjectManager.h"
#include "GameMapTile.h"

class GameMap
{
	std::vector<std::vector<GameMapTile>> mapTiles;
public:
	GameMap(ObjectManager& objectManager)
	{
		const auto row = std::vector<GameMapTile>(10, GameMapTile{ DirectX::XMFLOAT3{0, 0, 0}, objectManager, TerrainType::Dirt });
		mapTiles = std::vector<std::vector<GameMapTile>>(10, row);
	}
};