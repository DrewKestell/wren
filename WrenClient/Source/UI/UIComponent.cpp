#include "stdafx.h"
#include "UIComponent.h"

XMFLOAT3 UIComponent::GetWorldPosition() const
{
	auto worldPosition = localPosition;
	auto parentPtr = parent;
	while (parentPtr != nullptr)
	{
		worldPosition = XMFLOAT3Sum(worldPosition, parentPtr->localPosition);
		parentPtr = parentPtr->parent;
	}

	return worldPosition;
}

void UIComponent::Draw() {}