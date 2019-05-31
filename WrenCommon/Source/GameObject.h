#pragma once

#include "Utility.h"

class GameObject
{
	long id{ 0 };
	GameObject* parent{ nullptr };
	std::vector<GameObject*> children{ nullptr };
	
	// physics component?
	XMFLOAT3 localPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 movementVector{ 0.0f, 0.0f, 0.0f };
	float speed{ 60.0f };

	void Initialize(const long id, const XMFLOAT3 localPosition, const XMFLOAT3 scale);

	friend class ObjectManager;
public:
	void Update(const float deltaTime);
    void Translate(XMFLOAT3 vector) { localPosition = Utility::XMFLOAT3Sum(localPosition, vector); }
    GameObject* GetParent() const { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }
    XMFLOAT3 GetWorldPosition() const;
	XMFLOAT3 GetScale() const;
	void SetScale(const XMFLOAT3 scale);
	const long GetId() const;
	void SetId(const long id);
	XMFLOAT3 GetLocalPosition() const;
	void SetLocalPosition(XMFLOAT3 localPosition);
	XMFLOAT3 GetMovementVector() const;
	void SetMovementVector(const XMFLOAT3 movementVector);

	// components. i think this should be a map or vector? map<ComponentType, unsigned int>
	unsigned int statsComponentId{ 0 };
	unsigned int renderComponentId{ 0 };
};
