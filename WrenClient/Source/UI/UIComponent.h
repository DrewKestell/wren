#pragma once

#include "../ObjectManager.h"
#include "../GameObject.h"
#include "../Layer.h"

class UIComponent : public GameObject
{
protected:
	bool isVisible = false;
	const Layer uiLayer;	
public:
	UIComponent(ObjectManager& objectManager, const XMFLOAT3 localPosition, const Layer uiLayer)
		: GameObject{ objectManager, localPosition },
		uiLayer{ uiLayer }
	{
	}
	bool IsVisible() { return isVisible; }
	void SetVisible(bool isVisible) { this->isVisible = isVisible; }
	virtual ~UIComponent() = 0;
};