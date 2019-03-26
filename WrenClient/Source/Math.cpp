#include "Math.h"

DirectX::XMFLOAT3 XMFLOAT3Sum(DirectX::XMFLOAT3 l, DirectX::XMFLOAT3 r)
{
    return DirectX::XMFLOAT3(l.x + r.x, l.y + r.y, l.z + r.z);
}

bool DetectClick(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY, float mousePosX, float mousePosY)
{
	return mousePosX >= topLeftX && mousePosX <= bottomRightX && mousePosY >= topLeftY && mousePosY <= bottomRightY;
}