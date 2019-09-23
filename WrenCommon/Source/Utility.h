#pragma once

class Utility
{
public:
	static const bool DetectClick(const float topLeftX, const float topLeftY, const float bottomRightX, const float bottomRightY, const float mousePosX, const float mousePosY);
	static std::string ws2s(const std::wstring& wstr);
	static const char GetHotbarIndex(const float clientHeight, const float mousePosX, const float mousePosY);
	static const XMFLOAT3 MousePosToDirection(const float clientWidth, const float clientHeight, const float mouseX, const float mouseY);
	static const bool CheckOutOfBounds(const XMFLOAT3 pos);
	static void GetMapTileXYFromPos(const XMFLOAT3 pos, int& row, int& col);
	static const bool AreOnAdjacentOrDiagonalTiles(const XMFLOAT3 pos1, const XMFLOAT3 pos2);
	static const float Max(const float l, const float r);
	static const int Max(const int l, const int r);
};