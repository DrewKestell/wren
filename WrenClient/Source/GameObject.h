#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Utility.h"
#include "Layer.h"
#include "ObjectManager.h"

class GameObject
{
    DirectX::XMFLOAT3 localPosition;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
	ObjectManager& objectManager;
protected:
    GameObject(ObjectManager& objectManager, const DirectX::XMFLOAT3 localPosition)
        : localPosition{ localPosition },
		  objectManager{ objectManager }
    {
		objectManager.RegisterGameObject(*this);
    }
public:
    DirectX::XMFLOAT3 GetLocalPosition() { return localPosition; }
    
    void SetPosition(DirectX::XMFLOAT3 localPosition) { this->localPosition = localPosition; }
    void Translate(DirectX::XMFLOAT3 vector) { localPosition = XMFLOAT3Sum(localPosition, vector); }
    GameObject* GetParent() { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }

    DirectX::XMFLOAT3 GetWorldPosition();

    virtual void Draw() = 0;
	// TODO: how should the parent/child relationship work here? i think we should delete all children when the parent is deleted.
	~GameObject()
	{
		objectManager.DeleteGameObject(*this);
	}
};
