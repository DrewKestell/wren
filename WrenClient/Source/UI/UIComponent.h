#pragma once

#include "../GameObject.h"
#include "../Layer.h"

class UIComponent : public GameObject
{
protected:
	bool isVisible{ false };
	const Layer uiLayer{ Login };
public:
	UIComponent(const XMFLOAT3 localPosition, const XMFLOAT3 scale, const Layer uiLayer)
		: GameObject{ localPosition, scale },
		  uiLayer{ uiLayer }
	{
	}
	const bool IsVisible() const { return isVisible; }
	void SetVisible(const bool isVisible) { this->isVisible = isVisible; }
	virtual ~UIComponent() = 0;
};