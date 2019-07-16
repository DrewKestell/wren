#include "stdafx.h"
#include "AIComponent.h"

void AIComponent::Initialize(const unsigned int id, const long gameObjectId)
{
	this->id = id;
	this->gameObjectId = gameObjectId;
}