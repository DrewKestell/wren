#include "stdafx.h"
#include "GameObject.h"

void GameObject::Update(const float deltaTime, bool emulateHack)
{
	const auto testSpeed = emulateHack ? speed : speed * 1.5;

	// TODO: map bounds checking
	auto movementVec = XMLoadFloat3(&movementVector);
	movementVec *= testSpeed * deltaTime;
	
	auto positionVec = XMLoadFloat3(&localPosition);
	XMStoreFloat3(&localPosition, movementVec + positionVec);
}

void GameObject::Initialize(const long id, const XMFLOAT3 localPosition, const XMFLOAT3 scale)
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
        worldPosition = Utility::XMFLOAT3Sum(worldPosition, parentPtr->localPosition);
        parentPtr = parentPtr->parent;
    }
    
    return worldPosition;
}

const long GameObject::GetId() const { return id; }
void GameObject::SetId(const long id) { this->id = id; }
XMFLOAT3 GameObject::GetScale() const { return scale; }
void GameObject::SetScale(const XMFLOAT3 scale) { this->scale = scale; }
XMFLOAT3 GameObject::GetLocalPosition() const { return localPosition; }
void GameObject::SetLocalPosition(const XMFLOAT3 localPosition) { this->localPosition = localPosition; }
XMFLOAT3 GameObject::GetMovementVector() const { return movementVector; }
void GameObject::SetMovementVector(const XMFLOAT3 movementVector) { this->movementVector = movementVector; }