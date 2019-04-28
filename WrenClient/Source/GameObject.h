#pragma once

#include "Utility.h"
#include "PlayerController.h"
#include "RenderComponent.h"

class GameObject
{
	unsigned int id{ 0 };
	XMFLOAT3 localPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 0.0f, 0.0f, 0.0f };
	GameObject* parent{ nullptr };
    std::vector<GameObject*> children;

	// components. i think this should be a map or vector?
	unsigned int renderComponentId;
public:
	void Reset();
    void Translate(XMFLOAT3 vector) { localPosition = XMFLOAT3Sum(localPosition, vector); }
    GameObject* GetParent() const { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }
    XMFLOAT3 GetWorldPosition() const;
	XMFLOAT3 GetScale() const;
	void SetScale(const XMFLOAT3 scale);
	const unsigned int GetId() const;
	void SetId(const unsigned int id);
	XMFLOAT3 GetLocalPosition() const;
	void SetLocalPosition(XMFLOAT3 localPosition);
	unsigned int GetRenderComponentId() const;
	void SetRenderComponentId(const unsigned int renderComponentId);
};
