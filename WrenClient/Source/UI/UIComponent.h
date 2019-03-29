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
	virtual ~UIComponent() = 0;
};