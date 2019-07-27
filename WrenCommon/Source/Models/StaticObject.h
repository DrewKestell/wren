#pragma once

class StaticObject
{
	const unsigned int id;
	std::string* name;
	const unsigned int modelId;
	const unsigned int textureId;
	const XMFLOAT3 position;
public:
	StaticObject(const unsigned int id, std::string* name, const unsigned int modelId, const unsigned int textureId, const XMFLOAT3 position);
	const unsigned int GetId() const;
	std::string* GetName() const;
	const unsigned int GetModelId() const;
	const unsigned int GetTextureId() const;
	const XMFLOAT3 GetPosition() const;
};