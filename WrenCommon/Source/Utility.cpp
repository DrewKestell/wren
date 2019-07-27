#include "stdafx.h"
#include <Constants.h>
#include "Utility.h"

XMFLOAT3 Utility::XMFLOAT3Sum(const XMFLOAT3 l, const XMFLOAT3 r)
{
    return XMFLOAT3(l.x + r.x, l.y + r.y, l.z + r.z);
}

XMFLOAT2 Utility::XMFLOAT2Sum(const XMFLOAT2 l, const XMFLOAT2 r)
{
	return XMFLOAT2(l.x + r.x, l.y + r.y);
}

bool Utility::DetectClick(const float topLeftX, const float topLeftY, const float bottomRightX, const float bottomRightY, const float mousePosX, const float mousePosY)
{
	return mousePosX >= topLeftX && mousePosX <= bottomRightX && mousePosY >= topLeftY && mousePosY <= bottomRightY;
}

std::string Utility::ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

void Utility::PrintXMFLOAT3(const XMFLOAT3 vec)
{
	std::cout << vec.x << ", " << vec.y << ", " << vec.z << "\n";
}

const char Utility::GetHotbarIndex(const float clientHeight, const float mousePosX, const float mousePosY)
{
	if (mousePosX < 5.0f || mousePosX >= 405.0f || mousePosY < clientHeight - 45.0f || mousePosY > clientHeight - 5.0f)
		return -1;

	return (char)((mousePosX - 5) / 40);
}

const XMFLOAT3 Utility::AngleToDirection(const float angle)
{
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