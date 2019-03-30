#pragma once

#include <vector>
#include "UIInput.h"

class UIInputGroup : public Observer
{
	bool active = false;
	Layer uiLayer;
	std::vector<UIInput*> inputs;
public:
	UIInputGroup(
		EventHandler& eventHandler,
		Layer uiLayer
	) : 
		Observer{ eventHandler },
		uiLayer{ uiLayer }
	{
	}
	virtual bool HandleEvent(const Event& event);
	void AddInput(UIInput* input) { inputs.push_back(input); }
};