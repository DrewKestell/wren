#pragma once

class StaticObject
{
	const int id;
	const std::string name;
	const int modelId;
	const int textureId;
	const XMFLOAT3 position;
public:
	StaticObject(const int id, const char* name, const int modelId, const int textureId, const XMFLOAT3 position);
	const int GetId() const;
	const std::string GetName() const;
	const int GetModelId() const;
	const int GetTextureId() const;
	const XMFLOAT3 GetPosition() const;
};