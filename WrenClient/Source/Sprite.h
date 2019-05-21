#pragma once

#include "SpriteVertex.h"

class Sprite
{
	const unsigned int STRIDE{ sizeof(SpriteVertex) };
	const unsigned int OFFSET{ 0 };

	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	
public:
	Sprite(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11Device* device, const float originX, const float originY, const float width, const float height);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX projectionTransform);
};