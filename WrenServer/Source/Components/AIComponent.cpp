#include "stdafx.h"
#include "AIComponent.h"

void AIComponent::Initialize(const int id, const int gameObjectId)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
}

const int AIComponent::GetId() const { return id; }
const int AIComponent::GetGameObjectId() const { return gameObjectId; }