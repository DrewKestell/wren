#include "stdafx.h"
#include "GameObject.h"

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

XMFLOAT3 GameObject::GetScale() const { return scale; }
void GameObject::SetScale(const XMFLOAT3 scale) { this->scale = scale; }
void GameObject::SetPlayerController(PlayerController* playerController) { this->playerController = playerController; }
void GameObject::SetRenderComponent(RenderComponent * renderComponent) { this->renderComponent = renderComponent; }