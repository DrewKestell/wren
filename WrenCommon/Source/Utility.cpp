#include "stdafx.h"
#include "Utility.h"

XMFLOAT3 Utility::XMFLOAT3Sum(const XMFLOAT3 l, const XMFLOAT3 r)
{
    return XMFLOAT3(l.x + r.x, l.y + r.y, l.z + r.z);
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