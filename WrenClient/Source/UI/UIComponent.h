#pragma once

#include "Layer.h"
#include "Utility.h"
#include "EventHandling/Observer.h"

class UIComponent : public Observer
{
protected:
	std::vector<UIComponent*>& uiComponents;
	bool isVisible{ false };
	const Layer uiLayer;
	XMFLOAT2 localPosition;
	UIComponent* parent{ nullptr };
	std::vector<UIComponent*> children;
public:
	UIComponent(std::vector<UIComponent*>& uiComponents, const XMFLOAT2 localPosition, const Layer uiLayer);
	void Translate(XMFLOAT2 vector) { localPosition = Utility::XMFLOAT2Sum(localPosition, vector); }
	std::vector<UIComponent*>& GetChildren() { return children; }
	XMFLOAT2 GetWorldPosition() const;
	const bool IsVisible() const { return isVisible; }
	void SetVisible(const bool isVisible) { this->isVisible = isVisible; }
	UIComponent* GetParent() const { return parent; }
	void SetParent(UIComponent& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it 
	void AddChildComponent(UIComponent& child) { child.SetParent(*this); children.push_back(&child); }
	void ClearChildren();
	void SetLocalPosition(const XMFLOAT2 pos) { this->localPosition = pos; }
	virtual const std::string GetUIAbilityDragBehavior() const { return "NONE"; }
	virtual void Draw() = 0;
	~UIComponent();
};