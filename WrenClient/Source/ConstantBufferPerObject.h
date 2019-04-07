#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct ConstantBufferPerObject
{
	XMFLOAT4X4A mWorldViewProj;
};