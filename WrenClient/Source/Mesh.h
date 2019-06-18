#pragma once

#include "Vertex.h"

class Mesh
{
	int indexCount{ 0 };
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh(const std::string& path, ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize);
	const int GetIndexCount() const;
	ID3D11InputLayout* GetInputLayout() const;
	ID3D11Buffer* GetConstantBuffer() const;
	ID3D11SamplerState* GetSamplerState() const;
	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;
};	