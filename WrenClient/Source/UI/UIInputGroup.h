#pragma once

#include "UIInput.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"

class UIInputGroup : public Observer
{
	const Layer uiLayer{ Login };
	EventHandler& eventHandler;
	bool active{ false };
	std::vector<UIInput*> inputs;
public:
	UIInputGroup(const Layer uiLayer, EventHandler& eventHandler);
	const bool HandleEvent(const Event* const event) override;
	void AddInput(UIInput* input) { inputs.push_back(input); }
	~UIInputGroup();
};