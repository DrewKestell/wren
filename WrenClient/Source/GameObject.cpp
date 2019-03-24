#include "GameObject.h"

DirectX::XMFLOAT3 GameObject::GetWorldPosition()
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

GameObject::~GameObject() {}