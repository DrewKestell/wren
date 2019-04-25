#include "stdafx.h"
#include "Model.h"
#include "ConstantBufferPerObject.h"

void Model::Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform)
{
	
}

void Model::Translate(XMMATRIX matrix)
{
	//worldTransform = worldTransform * matrix;
}

XMFLOAT3 Model::GetPosition() const
{
	//XMFLOAT4X4 flt;
	//XMStoreFloat4x4(&flt, worldTransform);

	//return XMFLOAT3{ flt._41, flt._42, flt._43 };
	return XMFLOAT3{ 0.0f, 0.0f, 0.0f };
}

void Model::SetPosition(const XMFLOAT3 pos)
{
	//worldTransform = XMMatrixScaling(14.0f, 14.0f, 14.0f) * XMMatrixTranslation(pos.x, pos.y, pos.z);
}