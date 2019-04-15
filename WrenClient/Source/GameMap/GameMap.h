#pragma once

#include "GameMapTile.h"
#include "../Vertex.h"

const unsigned int MAP_WIDTH = 100;
const unsigned int MAP_HEIGHT = 100;
constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;
constexpr unsigned int INDEX_COUNT = MAP_SIZE * 6;

class GameMap
{
	std::vector<GameMapTile> mapTiles = std::vector<GameMapTile>(MAP_SIZE, GameMapTile{ TerrainType::Dirt });
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11SamplerState> samplerState;
	XMMATRIX worldTransform{ XMMatrixIdentity() };
	unsigned int stride{ sizeof(Vertex) };
	unsigned int offset{ 0 };
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;
public:
	GameMap(ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	GameMapTile& GetTile(const int row, const int col);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
};