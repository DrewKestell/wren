#include "stdafx.h"
#include "RenderComponent.h"
#include "ConstantBufferPerObject.h"

void RenderComponent::Reset()
{
	mesh = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	texture = nullptr;
}

void RenderComponent::Initialize(std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture)
{
	this->mesh = mesh;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->texture = texture;
}

void RenderComponent::Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, GameObject& gameObject, const float updateLag)
{
	// TODO: update GameObjects position based on state and updateLag
	auto pos = gameObject.GetWorldPosition();
	auto scale = gameObject.GetScale();
	auto worldTransform = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);

	auto constantBuffer = mesh->GetConstantBuffer();
	auto samplerState = mesh->GetSamplerState();
	auto vertexBuffer = mesh->GetVertexBuffer();

	// set InputLayout
	immediateContext->IASetInputLayout(mesh->GetInputLayout());

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
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	immediateContext->PSSetShaderResources(0, 1, &texture);

	// set VertexBuffer and IndexBuffer then Draw
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &STRIDE, &OFFSET);
	immediateContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}