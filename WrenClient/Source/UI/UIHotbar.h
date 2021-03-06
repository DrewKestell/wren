#pragma once

#include "UIComponent.h"
#include "UIAbility.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"

class UIHotbar : public UIComponent
{
	EventHandler& eventHandler;
	ID2D1SolidColorBrush* brush{ nullptr };
	char draggingIndex{ -1 };
	ComPtr<ID2D1RectangleGeometry> geometry[10];
	std::vector<UIAbility*> uiAbilities{ 10, nullptr };

public:
	UIHotbar(UIComponentArgs uiComponentArgs, EventHandler& eventHandler);
	void Initialize(ID2D1SolidColorBrush* brush);
	std::vector<UIAbility*>& GetUIAbilities();
	void CreateUIAbilityAtIndex(UIAbility* uiAbility, const int index);
	void Draw() override;
	const bool HandleEvent(const Event* const event) override;
	const std::string GetUIAbilityDragBehavior() const override;
};