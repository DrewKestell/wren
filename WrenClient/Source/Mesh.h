#pragma once

#include "Vertex.h"

class Mesh
{
	unsigned int stride{ sizeof(Vertex) };
	unsigned int offset{ 0 };
	int indexCount{ 0 };
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
public:
	Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
};	