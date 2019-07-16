#include "stdafx.h"
#include "Constants.h"
#include "GameObject.h"

void GameObject::Update()
{
	// TODO: map bounds checking
	auto movementVec = XMLoadFloat3(&movementVector);
	movementVec *= PLAYER_SPEED * UPDATE_FREQUENCY;
	
	auto positionVec = XMLoadFloat3(&localPosition);
	XMStoreFloat3(&localPosition, movementVec + positionVec);
}

void GameObject::Initialize(const long id, const XMFLOAT3 localPosition, const XMFLOAT3 scale, const bool isStatic, const int modelId, const int textureId)
{
	this->id = id;
	this->localPosition = localPosition;
	this->scale = scale;
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
        worldPosition = Utility::XMFLOAT3Sum(worldPosition, parentPtr->localPosition);
        parentPtr = parentPtr->parent;
    }
    
    return worldPosition;
}