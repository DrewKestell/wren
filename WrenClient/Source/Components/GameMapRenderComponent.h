#pragma once

#include <Constants.h>
#include "../Vertex.h"

// this is certainly not the right way to handle this...

static constexpr unsigned int INDEX_COUNT = MAP_SIZE * 6;
static constexpr unsigned int GAMEMAP_STRIDE = sizeof(Vertex);
static constexpr unsigned int GAMEMAP_OFFSET = 0;
static const XMMATRIX WORLD_TRANSFORM{ XMMatrixIdentity() };

class GameMapRenderComponent
{
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11SamplerState> samplerState;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
public:
	GameMapRenderComponent(ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
};