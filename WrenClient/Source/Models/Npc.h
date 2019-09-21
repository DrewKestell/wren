#pragma once

class Npc
{
	const int id;
	const std::string name;
	const int modelId;
	const int textureId;
	const float speed;
public:
	Npc(const int id, const char* name, const int modelId, const int textureId, const float speed);
	const int GetId() const;
	const std::string GetName() const;
	const int GetModelId() const;
	const int GetTextureId() const;
	const float GetSpeed() const;
};