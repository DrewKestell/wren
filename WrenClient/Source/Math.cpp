#include "Math.h"

DirectX::XMFLOAT3 XMFLOAT3Sum(DirectX::XMFLOAT3 l, DirectX::XMFLOAT3 r)
{
    return DirectX::XMFLOAT3(l.x + r.x, l.y + r.y, l.z + r.z);
}