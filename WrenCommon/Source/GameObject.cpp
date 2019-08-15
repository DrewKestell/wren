#include "stdafx.h"
#include "Constants.h"
#include "GameObject.h"

void GameObject::Update()
{
	auto movementVec = XMLoadFloat3(&movementVector);
	movementVec *= speed * UPDATE_FREQUENCY;
	
	auto positionVec = XMLoadFloat3(&localPosition);
	XMStoreFloat3(&localPosition, movementVec + positionVec);
}

void GameObject::Initialize(const long id, GameObjectType type, const XMFLOAT3 localPosition, const XMFLOAT3 scale, const float speed, const bool isStatic, const int modelId, const int textureId)
{
	this->id = id;
	this->type = type;
	this->localPosition = localPosition;
	this->scale = scale;
	this->speed = speed;
	this->isStatic = isStatic;
	this->modelId = modelId;
	this->textureId = textureId;
}

XMFLOAT3 GameObject::GetWorldPosition() const
{
    auto worldPosition = localPosition;
    auto parentPtr = parent;
    while (parentPtr != nullptr)
    {
        worldPosition = worldPosition + parentPtr->localPosition;
        parentPtr = parentPtr->parent;
    }
    
    return worldPosition;
}