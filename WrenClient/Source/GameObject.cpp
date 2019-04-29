#include "stdafx.h"
#include "GameObject.h"

void GameObject::Update(const float deltaTime)
{
	// TODO: map bounds checking
	auto movementVec = XMLoadFloat3(&movementVector);
	movementVec *= speed * deltaTime;
	
	auto positionVec = XMLoadFloat3(&localPosition);
	XMStoreFloat3(&localPosition, movementVec + positionVec);
}

void GameObject::Initialize(const unsigned int id, const XMFLOAT3 localPosition, const XMFLOAT3 scale)
{
	this->id = id;
	this->localPosition = localPosition;
	this->scale = scale;
}

XMFLOAT3 GameObject::GetWorldPosition() const
{
    auto worldPosition = localPosition;
    auto parentPtr = parent;
    while (parentPtr != nullptr)
    {
        worldPosition = XMFLOAT3Sum(worldPosition, parentPtr->localPosition);
        parentPtr = parentPtr->parent;
    }
    
    return worldPosition;
}

const unsigned int GameObject::GetId() const { return id; }
void GameObject::SetId(const unsigned int id) { this->id = id; }
XMFLOAT3 GameObject::GetScale() const { return scale; }
void GameObject::SetScale(const XMFLOAT3 scale) { this->scale = scale; }
XMFLOAT3 GameObject::GetLocalPosition() const { return localPosition; }
void GameObject::SetLocalPosition(const XMFLOAT3 localPosition) { this->localPosition = localPosition; }
unsigned int GameObject::GetRenderComponentId() const { return renderComponentId; }
void GameObject::SetRenderComponentId(const unsigned int renderComponentId) { this->renderComponentId = renderComponentId; }