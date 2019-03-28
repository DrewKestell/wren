#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Math.h"
#include "Layer.h"

class GameObject
{
    DirectX::XMFLOAT3 localPosition;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
protected:
    GameObject(const DirectX::XMFLOAT3 localPosition)
        : localPosition{ localPosition }
    {
    }
public:
    DirectX::XMFLOAT3 GetLocalPosition() { return localPosition; }
    
    void SetPosition(DirectX::XMFLOAT3 localPosition) { this->localPosition = localPosition; }
    void Translate(DirectX::XMFLOAT3 vector) { localPosition = XMFLOAT3Sum(localPosition, vector); } // TODO: how do you overload compound assignment operator (+=)?
    GameObject* GetParent() { return parent; }
    void SetParent(GameObject* parent) { /*delete(this->parent);*/ this->parent = parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject* child) { child->SetParent(this); children.push_back(child); }

    DirectX::XMFLOAT3 GetWorldPosition();

    virtual void Draw(const Layer layer) = 0;
};
