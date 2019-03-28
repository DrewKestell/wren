#pragma once

#include "../GameObject.h"
#include "../Layer.h"

class UIComponent : public GameObject
{
protected:
	const Layer uiLayer;
public:
	UIComponent(const DirectX::XMFLOAT3 localPosition, const Layer uiLayer)
		: GameObject{ localPosition },
		uiLayer{ uiLayer }
	{
	}
	virtual ~UIComponent() = 0;
};