#include "stdafx.h"
#include "AIComponent.h"

void AIComponent::Initialize(const int id, const int gameObjectId)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
}