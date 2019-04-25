#pragma once

#include "Vertex.h"

const unsigned int STRIDE{ sizeof(Vertex) };
const unsigned int OFFSET{ 0 };

class Mesh
{
	int indexCount{ 0 };
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
public:
	Mesh(const std::string& path, ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
	const int GetIndexCount() const;
	ID3D11InputLayout* GetInputLayout() const;
	ID3D11Buffer* GetConstantBuffer() const;
	ID3D11SamplerState* GetSamplerState() const;
	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;
};	