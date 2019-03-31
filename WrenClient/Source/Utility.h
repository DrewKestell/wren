#pragma once

#include <DirectXMath.h>
#include <string>

DirectX::XMFLOAT3 XMFLOAT3Sum(DirectX::XMFLOAT3 l, DirectX::XMFLOAT3 r);
bool DetectClick(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY, float mousePosX, float mousePosY);
std::string ws2s(const std::wstring& wstr);