#pragma once


class Npc
{
	const unsigned int id;
	std::string* name;
	const unsigned int modelId;
	const unsigned int textureId;
public:
	Npc(const unsigned int id, std::string* name, const unsigned int modelId, const unsigned int textureId);
	const unsigned int GetId() const;
	std::string* GetName() const;
	const unsigned int GetModelId() const;
	const unsigned int GetTextureId() const;
};