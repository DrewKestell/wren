#pragma once

#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class Character
{
public:
	Character(const int id, const std::string& name, const int accountId, const XMFLOAT3 position)
		: id{ id },
		  name{ name },
		  accountId{ accountId },
		  position{ position }
	{
	}
	const int id{ 0 };
	const std::string& name;
	const int accountId{ 0 };
	const XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
};