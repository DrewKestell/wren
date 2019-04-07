#pragma once

#include "UIInput.h"
#include "../EventHandling/Observer.h"

class UIInputGroup : public Observer
{
	bool active = false;
	Layer uiLayer;
	std::vector<UIInput*> inputs;
public:
	UIInputGroup(Layer uiLayer)
		: uiLayer{ uiLayer }
	{
	}
	virtual bool HandleEvent(const Event* event);
	void AddInput(UIInput* input) { inputs.push_back(input); }
};