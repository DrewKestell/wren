#include "stdafx.h"
#include "GameObject.h"

void GameObject::Update()
{
	const auto position = GetWorldPosition();

	// first check if the destination is reached
	if (movementVector != VEC_ZERO)
	{
		auto deltaX = std::abs(position.x - destination.x);
		auto deltaZ = std::abs(position.z - destination.z);

		if (deltaX < 1.0f && deltaZ < 1.0f)
		{
			localPosition = destination;
			movementVector = VEC_ZERO;
			destination = VEC_ZERO;
		}
	}

	auto movementVec = XMLoadFloat3(&movementVector);
	movementVec *= speed * UPDATE_FREQUENCY;
	
	auto positionVec = XMLoadFloat3(&localPosition);
	XMStoreFloat3(&localPosition, movementVec + positionVec);
}

void GameObject::Initialize(const int id, GameObjectType type, std::string* name, XMFLOAT3 localPosition, const XMFLOAT3 scale, const float speed, const bool isStatic, const int modelId, const int textureId)
{
	this->id = id;
	this->type = type;
	this->name = name;
	this->localPosition = localPosition;
	this->scale = scale;
	this->speed = speed;
	this->isStatic = isStatic;
	this->modelId = modelId;
	this->textureId = textureId;
}

XMFLOAT3 GameObject::GetWorldPosition() const
{
    auto worldPosition = localPosition;
    auto parentPtr = parent;
    while (parentPtr != nullptr)
    {
        worldPosition = worldPosition + parentPtr->localPosition;
        parentPtr = parentPtr->parent;
    }
    
    return worldPosition;
}

const int GameObject::GetId() const { return id; }

const GameObjectType GameObject::GetType() const { return type; }
