#include "stdafx.h"
#include "Npc.h"

Npc::Npc(const int id, const char* name, const int modelId, const int textureId, const float speed)
	: id{ id },
	  name{ name },
	  modelId{ modelId },
	  textureId{ textureId },
	  speed{ speed }
{
}

const int Npc::GetId() const { return id; }
const std::string Npc::GetName() const { return name; }
const int Npc::GetModelId() const { return modelId; }
const int Npc::GetTextureId() const { return textureId; }
const float Npc::GetSpeed() const { return speed; }