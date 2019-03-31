#pragma once

#include "../GameObject.h"
#include "../Layer.h"

class UIComponent : public GameObject
{
protected:
	bool isVisible = false;
	const Layer uiLayer;	
public:
	UIComponent(ObjectManager& objectManager, const DirectX::XMFLOAT3 localPosition, const Layer uiLayer)
		: GameObject{ objectManager, localPosition },
		uiLayer{ uiLayer }
	{
	}
	bool IsVisible() { return isVisible; }
	void SetVisible(bool isVisible) { this->isVisible = isVisible; }
	virtual ~UIComponent() = 0;
};