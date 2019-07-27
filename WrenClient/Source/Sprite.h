#pragma once

#include "SpriteVertex.h"

static constexpr unsigned int SPRITE_STRIDE = sizeof(SpriteVertex);
static constexpr unsigned int SPRITE_OFFSET = 0;

class Sprite
{
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	
public:
	Sprite(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11Device* device, const float originX, const float originY, const float width, const float height);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX projectionTransform);
};