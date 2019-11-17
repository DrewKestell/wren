#include "stdafx.h"
#include <Constants.h>
#include "Utility.h"

const bool Utility::DetectClick(const float topLeftX, const float topLeftY, const float bottomRightX, const float bottomRightY, const float mousePosX, const float mousePosY)
{
	return mousePosX >= topLeftX && mousePosX <= bottomRightX && mousePosY >= topLeftY && mousePosY <= bottomRightY;
}

std::string Utility::ws2s(const std::wstring& wstr)
{
	if (wstr.empty())
		return std::string();

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

	return strTo;
}

std::wstring Utility::s2ws(const std::string& str)
{
	if (str.empty())
		return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

	return wstrTo;
}

const char Utility::GetHotbarIndex(const float clientHeight, const float mousePosX, const float mousePosY)
{
	if (mousePosX < 5.0f || mousePosX >= 405.0f || mousePosY < clientHeight - 45.0f || mousePosY > clientHeight - 5.0f)
		return -1;

	return static_cast<char>((mousePosX - 5) / 40);
}

const char Utility::GetInventoryIndex(const float inventoryPosX, const float inventoryPosY, const float itemPosX, const float itemPosY)
{
	const auto initialX = inventoryPosX + 5.0f;
	const auto initialY = inventoryPosY + 25.0f;

	if (itemPosX >= initialX && itemPosX <= initialX + 180.0f && itemPosY >= initialY && itemPosY <= initialY + 180.0f)
	{
		const auto deltaX = itemPosX - initialX;
		const auto deltaY = itemPosY - initialY;

		const auto row = static_cast<int>(deltaY) / static_cast<int>(45.0f);
		if (deltaY - (row * 45) > 40.0f)
			return -1;
		const auto col = static_cast<int>(deltaX) / static_cast<int>(45.0f);
		if (deltaX - (col * 45) > 40.0f)
			return -1;

		return col + (row * 4);
	}

	return -1;
}

const XMFLOAT3 Utility::MousePosToDirection(const float clientWidth, const float clientHeight, const float mouseX, const float mouseY)
{
	const auto centerPoint = XMVECTOR{ clientWidth / 2, clientHeight / 2, 0.0f, 0.0f };
	const auto clickPoint = XMVECTOR{ mouseX, mouseY, 0.0f, 0.0f };
	const auto clickVec = XMVectorSubtract(centerPoint, clickPoint);
	const auto upVec = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
	const auto angleVec = XMVectorATan2(XMVector3Dot(upVec, clickVec), XMVector3Cross(upVec, clickVec));
	XMFLOAT4 angleFloat;
	XMStoreFloat4(&angleFloat, angleVec);
	const auto angle = XMConvertToDegrees(angleFloat.z) + 180.0f;

	if (angle >= 337.5 || angle <= 22.5)
		return VEC_SOUTHWEST;
	else if (angle > 22.5 && angle < 67.5)
		return VEC_SOUTH;
	else if (angle > 67.5 && angle < 112.5)
		return VEC_SOUTHEAST;
	else if (angle > 112.5 && angle < 157.5)
		return VEC_EAST;
	else if (angle > 157.5 && angle < 202.5)
		return VEC_NORTHEAST;
	else if (angle > 202.5 && angle < 247.5)
		return VEC_NORTH;
	else if (angle > 247.5 && angle < 292.5)
		return VEC_NORTHWEST;
	else
		return VEC_WEST;
}

const bool Utility::CheckOutOfBounds(const XMFLOAT3 pos)
{
	const auto maxX = TILE_SIZE * MAP_WIDTH - 1;
	const auto maxZ = TILE_SIZE * MAP_HEIGHT - 1;

	return pos.x < 0.0f || pos.z < 0.0f || pos.x > maxX || pos.z > maxZ;
}

void Utility::GetMapTileXYFromPos(const XMFLOAT3 pos, int& row, int& col)
{
	row = static_cast<int>(pos.x) / static_cast<int>(TILE_SIZE);
	col = static_cast<int>(pos.z) / static_cast<int>(TILE_SIZE);
}

const bool Utility::AreOnAdjacentOrDiagonalTiles(const XMFLOAT3 pos1, const XMFLOAT3 pos2)
{
	int row1, col1;
	Utility::GetMapTileXYFromPos(pos1, row1, col1);

	int row2, col2;
	Utility::GetMapTileXYFromPos(pos2, row2, col2);

	return std::abs(row1 - row2) <= 1 && std::abs(col1 - col2) <= 1;
}
