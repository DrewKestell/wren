#pragma once

#include "GameMapTile.h"
#include "../Vertex.h"

const unsigned int MAP_WIDTH = 100;
const unsigned int MAP_HEIGHT = 100;
constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;
constexpr unsigned int INDEX_COUNT = MAP_SIZE * 6;

class GameMap
{
	XMMATRIX worldTransform = XMMatrixIdentity();
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11SamplerState* samplerState = nullptr;
	std::vector<GameMapTile> mapTiles = std::vector<GameMapTile>(MAP_SIZE, GameMapTile{TerrainType::Dirt});
public:
	GameMap(ID3D11Device* device, BYTE* vertexShaderBuffer, int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	GameMapTile& GetTile(int row, int col);
	void Draw(ID3D11DeviceContext* immediateContext, XMMATRIX viewTransform, XMMATRIX projectionTransform);
};