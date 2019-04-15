#pragma once

XMFLOAT3 XMFLOAT3Sum(const XMFLOAT3 l, const XMFLOAT3 r);
bool DetectClick(const float topLeftX, const float topLeftY, const float bottomRightX, const float bottomRightY, const float mousePosX, const float mousePosY);
std::string ws2s(const std::wstring& wstr);
void PrintXMFLOAT3(const XMFLOAT3 vec);