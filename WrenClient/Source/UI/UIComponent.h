#pragma once

#include "../ObjectManager.h"
#include "../GameObject.h"
#include "../Layer.h"

class UIComponent : public GameObject
{
protected:
	bool isVisible{ false };
	const Layer uiLayer{ Login };
public:
	UIComponent(ObjectManager& objectManager, const XMFLOAT3 localPosition, const Layer uiLayer)
		: GameObject{ objectManager, localPosition },
		  uiLayer{ uiLayer }
	{
	}
	const bool IsVisible() const { return isVisible; }
	void SetVisible(const bool isVisible) { this->isVisible = isVisible; }
	virtual ~UIComponent() = 0;
};