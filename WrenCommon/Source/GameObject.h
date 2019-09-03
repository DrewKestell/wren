#pragma once

#include <Constants.h>
#include "Utility.h"
#include "GameObjectType.h"

class GameObject
{
	float speed{ 0.0f };
	GameObject* parent{ nullptr };
	std::vector<GameObject*> children;
	int id{ 0 };
	GameObjectType type{ GameObjectType::Uninitialized };

	void Initialize(const int id, GameObjectType type, std::string* name, const XMFLOAT3 localPosition, const XMFLOAT3 scale, const float speed, const bool isStatic, const int modelId, const int textureId);

	friend class ObjectManager;
public:
	void Update();
    void Translate(XMFLOAT3 vector) { localPosition = localPosition + vector; }
    GameObject* GetParent() const { return parent; }
    void SetParent(GameObject& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it here.
    std::vector<GameObject*>& GetChildren() { return children; }
    void AddChildComponent(GameObject& child) { child.SetParent(*this); children.push_back(&child); }
    XMFLOAT3 GetWorldPosition() const;
	const int GetId() const;
	const GameObjectType GetType() const;

	std::string* name{ nullptr };

	// physics component?
	XMFLOAT3 localPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 0.0f, 0.0f, 0.0f };
	bool isStatic{ false };
	XMFLOAT3 movementVector{ VEC_ZERO };
	XMFLOAT3 destination{ VEC_ZERO };

	// only really needed on the server. separate component?
	int modelId{ -1 };
	int textureId{ -1 };

	// components. i think this should be a map or vector? map<ComponentType, unsigned int>
	int statsComponentId{ -1 };
	int renderComponentId{ -1 };
	int aiComponentId{ -1 };
	int playerComponentId{ -1 };
	int skillComponentId{ -1 };
};
