#include "stdafx.h"
#include "Sprite.h"
#include "ConstantBufferPerObject.h"

Sprite::Sprite(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11Device* device, const float originX, const float originY, const float width, const float height)
	: vertexShader{ vertexShader },
	  pixelShader{ pixelShader },
	  texture{ texture }
{
	SpriteVertex vertices[4];

	// top left
	SpriteVertex topLeftVertex;
	topLeftVertex.Position = XMFLOAT3{ originX - (width / 2), originY + (height / 2), 0.0f };
	topLeftVertex.TexCoords = XMFLOAT2{ 0.0f, 0.0f };
	vertices[0] = topLeftVertex;

	// top right
	SpriteVertex topRightVertex;
	topRightVertex.Position = XMFLOAT3{ originX + (width / 2), originY + (height / 2), 0.0f };
	topRightVertex.TexCoords = XMFLOAT2{ 1.0f, 0.0f };
	vertices[1] = topRightVertex;

	// bottom right
	SpriteVertex bottomRightVertex;
	bottomRightVertex.Position = XMFLOAT3{ originX + (width / 2), originY - (height / 2), 0.0f };
	bottomRightVertex.TexCoords = XMFLOAT2{ 1.0f, 1.0f };
	vertices[2] = bottomRightVertex;

	// bottom left
	SpriteVertex bottomLeftVertex;
	bottomLeftVertex.Position = XMFLOAT3{ originX - (width / 2), originY - (height / 2), 0.0f };
	bottomLeftVertex.TexCoords = XMFLOAT2{ 0.0f, 1.0f };
	vertices[3] = bottomLeftVertex;

	const unsigned int indices[6]{ 0, 1, 3, 1, 2, 3 };

	// create SamplerState - TODO: pass this in as param
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
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

	device->CreateSamplerState(&samplerDesc, samplerState.ReleaseAndGetAddressOf());

	// create constant buffer - TODO: can you pass this in as param?
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(ConstantBufferPerObject);
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());

	// create
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device->CreateInputLayout(ied, ARRAYSIZE(ied), vertexShaderBuffer, vertexShaderSize, inputLayout.ReleaseAndGetAddressOf());

	// create vertex buffer using existing bufferDesc
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = UINT{ sizeof(SpriteVertex) * 4 };
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;

	device->CreateBuffer(&bufferDesc, &vertexData, vertexBuffer.ReleaseAndGetAddressOf());

	// create index buffer using existing bufferDesc
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = UINT{ sizeof(unsigned int) * 6 };

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;

	device->CreateBuffer(&bufferDesc, &indexData, indexBuffer.ReleaseAndGetAddressOf());
}

void Sprite::Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX projectionTransform)
{
	// set InputLayout
	immediateContext->IASetInputLayout(inputLayout.Get());

	// map ConstantBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto pCB = reinterpret_cast<ConstantBufferPerObject*>(mappedResource.pData);
	XMStoreFloat4x4(&pCB->mWorldViewProj, XMMatrixTranspose(projectionTransform));
	immediateContext->Unmap(constantBuffer.Get(), 0);

	// setup VertexShader
	immediateContext->VSSetShader(vertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// setup PixelShader
	immediateContext->PSSetShader(pixelShader, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	immediateContext->PSSetShaderResources(0, 1, &texture);

	// set VertexBuffer and IndexBuffer then Draw
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &SPRITE_STRIDE, &SPRITE_OFFSET);
	immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->DrawIndexed(6, 0, 0);
}