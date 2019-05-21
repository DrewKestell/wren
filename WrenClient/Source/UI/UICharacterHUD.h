#pragma once

#include "UIComponent.h"

class UICharacterHUD : public UIComponent
{
public:
	UICharacterHUD();
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
};