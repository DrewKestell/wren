#include "stdafx.h"

std::ostream& operator<< (std::ostream& out, const XMFLOAT3& vec)
{
	out << vec.x << ", " << vec.y << ", " << vec.z;

	return out;
}

DirectX::XMFLOAT3 operator+ (const DirectX::XMFLOAT3& l, const DirectX::XMFLOAT3& r)
{
	return XMFLOAT3{ l.x + r.x, l.y + r.y, l.z + r.z };
}

DirectX::XMFLOAT2 operator+ (const DirectX::XMFLOAT2& l, const DirectX::XMFLOAT2& r)
{
	return XMFLOAT2{ l.x + r.x, l.y + r.y };
}

bool operator== (const DirectX::XMFLOAT3& l, const DirectX::XMFLOAT3& r)
{
	return l.x == r.x && l.y == r.y && l.z == r.z;
}

bool operator!= (const DirectX::XMFLOAT3& l, const DirectX::XMFLOAT3& r)
{
	return !(l == r);
}