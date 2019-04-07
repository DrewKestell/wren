#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	: indexCount{ (int)indices.size() }
{
	// create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();

	device->CreateBuffer(&bufferDesc, &vertexData, &vertexBuffer);

	// create index buffer
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();

	device->CreateBuffer(&bufferDesc, &indexData, &indexBuffer);
}

void Mesh::Draw(ID3D11DeviceContext* immediateContext, XMMATRIX viewTransform, XMMATRIX projectionTransform)
{
	// set VertexBuffer and IndexBuffer then Draw
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->DrawIndexed(indexCount, 0, 0);
}