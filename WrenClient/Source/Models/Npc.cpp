#include "stdafx.h"
#include "Npc.h"

Npc::Npc(const unsigned int id, std::string* name, const unsigned int modelId, const unsigned int textureId)
	: id{ id },
	  name{ name },
	  modelId{ modelId },
	  textureId{ textureId }
{
}

const unsigned int Npc::GetId() const { return id; }
std::string* Npc::GetName() const { return name; }
const unsigned int Npc::GetModelId() const { return modelId; }
const unsigned int Npc::GetTextureId() const { return textureId; }