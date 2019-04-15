#include "stdafx.h"
#include "GameMap.h"
#include "../ConstantBufferPerObject.h"

const unsigned int TILE_WIDTH = 30;
const unsigned int TILE_HEIGHT = 30;

// this can be optimized. there are more shared vertices here (between tiles).
GameMap::GameMap(ID3D11Device* device, BYTE* vertexShaderBuffer, int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
	: vertexShader{ vertexShader },
	  pixelShader{ pixelShader }, 
	  texture{ texture }
{
	std::vector<Vertex> vertices = std::vector<Vertex>(MAP_SIZE * 4);
	std::vector<unsigned int> indices = std::vector<unsigned int>(MAP_SIZE * 6, 0);

	for (auto i = 0; i < MAP_SIZE; i++)
	{
		const auto row = i / MAP_HEIGHT;
		const auto col = i % MAP_WIDTH;

		const float x = (col * TILE_WIDTH) - (float)TILE_WIDTH / 2;
		const float z = (row * TILE_HEIGHT) - (float)TILE_HEIGHT / 2;

		const auto bottomLeft = i * 4;
		const auto topLeft = (i * 4) + 1;
		const auto topRight = (i * 4) + 2;
		const auto bottomRight = (i * 4) + 3;

		vertices[bottomLeft] = Vertex{ XMFLOAT3{ x, 0.0f, z }, XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT2{0.0f, 0.0f} };
		vertices[topLeft] = Vertex{ XMFLOAT3{ x, 0.0f, z + TILE_HEIGHT }, XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT2{1.0f, 0.0f} };
		vertices[topRight] = Vertex{ XMFLOAT3{ x + TILE_WIDTH, 0.0f, z + TILE_HEIGHT }, XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT2{1.0f, 1.0f} };
		vertices[bottomRight] = Vertex{ XMFLOAT3{ x + TILE_WIDTH, 0.0f, z }, XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT2{0.0f, 1.0f} };

		indices[i * 6] = bottomLeft;
		indices[(i * 6) + 1] = topLeft;
		indices[(i * 6) + 2] = bottomRight;
		indices[(i * 6) + 3] = topLeft;
		indices[(i * 6) + 4] = topRight;
		indices[(i * 6) + 5] = bottomRight;
	}

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

	// create constant buffer
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(ConstantBufferPerObject);

	device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);

	// create SamplerState
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &samplerState);

	// create InputLayout
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device->CreateInputLayout(ied, ARRAYSIZE(ied), vertexShaderBuffer, vertexShaderSize, &inputLayout);
}

GameMapTile& GameMap::GetTile(int row, int col)
{
	return mapTiles[(row * MAP_WIDTH) + col];
}

void GameMap::Draw(ID3D11DeviceContext* immediateContext, XMMATRIX viewTransform, XMMATRIX projectionTransform)
{
	// set InputLayout
	immediateContext->IASetInputLayout(inputLayout);

	// map ConstantBuffer
	auto worldViewProjection = worldTransform * viewTransform * projectionTransform;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto pCB = reinterpret_cast<ConstantBufferPerObject*>(mappedResource.pData);
	XMStoreFloat4x4(&pCB->mWorldViewProj, XMMatrixTranspose(worldViewProjection));
	immediateContext->Unmap(constantBuffer, 0);

	// setup VertexShader
	immediateContext->VSSetShader(vertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	// setup PixelShader
	immediateContext->PSSetShader(pixelShader, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, &texture);
	immediateContext->PSSetSamplers(0, 1, &samplerState);

	// set VertexBuffer and IndexBuffer then Draw
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->DrawIndexed(INDEX_COUNT, 0, 0);
}