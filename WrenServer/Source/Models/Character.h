#pragma once

#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class Character
{
	const int id;
	const std::string& name;
	const int accountId;
	const XMFLOAT3 position;
	const int modelId;
	const int textureId;

public:
	Character(const int id, const std::string& name, const int accountId, const XMFLOAT3 position, const int modelId, const int textureId)
		: id{ id },
		  name{ name },
		  accountId{ accountId },
		  position{ position },
		  modelId{ modelId },
		  textureId{ textureId }
	{
	}
	
	const int GetId() const { return id; }
	const std::string& GetName() const { return name; }
	const int GetAccountId() const { return accountId; }
	const XMFLOAT3 GetPosition() const { return position; }
	const int GetModelId() const { return modelId; }
	const int GetTextureId() const { return textureId; }
};