#include "stdafx.h"
#include "StaticObject.h"

StaticObject::StaticObject(const unsigned int id, std::string* name, const unsigned int modelId, const unsigned int textureId, const XMFLOAT3 position)
	: id{ id },
	  name{ name },
	  modelId{ modelId },
	  textureId{ textureId },
	  position{ position }
{
}

const unsigned int StaticObject::GetId() const { return id; }
std::string* StaticObject::GetName() const { return name; }
const unsigned int StaticObject::GetModelId() const { return modelId; }
const unsigned int StaticObject::GetTextureId() const { return textureId; }
const XMFLOAT3 StaticObject::GetPosition() const { return position; }
