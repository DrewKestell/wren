#include "stdafx.h"
#include "StaticObject.h"

StaticObject::StaticObject(const int id, std::string* name, const int modelId, const int textureId, const XMFLOAT3 position)
	: id{ id },
	  name{ name },
	  modelId{ modelId },
	  textureId{ textureId },
	  position{ position }
{
}

const int StaticObject::GetId() const { return id; }
std::string* StaticObject::GetName() const { return name; }
const int StaticObject::GetModelId() const { return modelId; }
const int StaticObject::GetTextureId() const { return textureId; }
const XMFLOAT3 StaticObject::GetPosition() const { return position; }
