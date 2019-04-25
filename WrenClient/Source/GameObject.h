#pragma once

#include "ObjectManager.h"
#include "Utility.h"
#include "PlayerController.h"
#include "RenderComponent.h"

class GameObject
{
	XMFLOAT3 localPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 0.0f, 0.0f, 0.0f };
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;
	ObjectManager& objectManager;

	// components. i think this should be a map or vector?
	PlayerController* playerController;
	RenderComponent* renderComponent;
protected:
    GameObject(ObjectManager& objectManager, const XMFLOAT3 localPosition, const XMFLOAT3 scale)
        : localPosition{ localPosition },
		  scale{ scale },
		  objectManager{ objectManager }
    {
		objectManager.RegisterGameObject(*this);
    }
public:
    XMFLOAT3 GetLocalPosition() const { return localPosition; }
    
    void SetPosition(XMFLOAT3 localPosition) { this->localPosition = localPosition; }
    void Translate(XMFLOAT3 vector) { localPosition = XMFLOAT3Sum(localPosition, vector); }
    GameObject* GetParent() const { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }
    XMFLOAT3 GetWorldPosition() const;
	XMFLOAT3 GetScale() const;
	void SetScale(const XMFLOAT3 scale);
	void SetPlayerController(PlayerController* playerController);
	void SetRenderComponent(RenderComponent* renderComponent);

	// TODO: how should the parent/child relationship work here? i think we should delete all children when the parent is deleted. Prolly a shared pointer?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	~GameObject()
	{
		objectManager.DeleteGameObject(*this);
	}
};
