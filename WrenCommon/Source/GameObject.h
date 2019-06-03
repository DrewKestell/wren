#pragma once

#include "Utility.h"

class GameObject
{
	GameObject* parent{ nullptr };
	std::vector<GameObject*> children{ nullptr };
	
	void Initialize(const long id, const XMFLOAT3 localPosition, const XMFLOAT3 scale);

	friend class ObjectManager;
public:
	void Update();
    void Translate(XMFLOAT3 vector) { localPosition = Utility::XMFLOAT3Sum(localPosition, vector); }
    GameObject* GetParent() const { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }
    XMFLOAT3 GetWorldPosition() const;

	long id{ 0 };

	// physics component?
	XMFLOAT3 localPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 movementVector{ 0.0f, 0.0f, 0.0f };

	// components. i think this should be a map or vector? map<ComponentType, unsigned int>
	unsigned int statsComponentId{ 0 };
	unsigned int renderComponentId{ 0 };
};
