#pragma once

#include "Layer.h"
#include "Utility.h"
#include "EventHandling/Observer.h"

class UIComponent : public Observer
{
protected:
	std::vector<UIComponent*>& uiComponents;
	const Layer uiLayer;
	XMFLOAT2 localPosition;
	UIComponent* parent{ nullptr };
	std::vector<UIComponent*> children;
public:
	bool isVisible{ false };
	unsigned int zIndex;
	UIComponent(std::vector<UIComponent*>& uiComponents, const XMFLOAT2 localPosition, const Layer uiLayer, const unsigned int zIndex);
	void Translate(XMFLOAT2 vector) { localPosition = localPosition + vector; }
	std::vector<UIComponent*>& GetChildren() { return children; }
	XMFLOAT2 GetWorldPosition() const;
	UIComponent* GetParent() const { return parent; }
	void SetParent(UIComponent& parent) { /*delete(this->parent);*/ this->parent = &parent; } // TODO: i think parent should be a shared_pointer, because another gameobject could have a reference to it, and we can't delete it 
	void AddChildComponent(UIComponent& child) { child.SetParent(*this); children.push_back(&child); }
	void RemoveChildComponent(UIComponent* child)
	{
		for (auto i = 0; i < children.size(); i++)
		{
			if (children.at(i) == child)
				children.erase(children.begin() + i);
		}
	}
	void ClearChildren();
	void EmptyChildren();
	void SetLocalPosition(const XMFLOAT2 pos) { this->localPosition = pos; }
	virtual const std::string GetUIAbilityDragBehavior() const { return "NONE"; }
	virtual const std::string GetUIItemDragBehavior() const { return "NONE"; }
	virtual const std::string GetUIItemRightClickBehavior() const { return "NONE"; }
	virtual void Draw() = 0;
	~UIComponent();
};