#pragma once

#include "Layer.h"
#include "Utility.h"
#include "EventHandling/Observer.h"
#include "../DeviceResources.h"
#include "UIComponentArgs.h"

class UIComponent : public Observer
{
protected:
	DX::DeviceResources* deviceResources;
	std::vector<UIComponent*>& uiComponents;
	const Layer uiLayer;
	XMFLOAT2 localPosition{ 0.0f, 0.0f };
	UIComponent* parent{ nullptr };
	std::vector<UIComponent*> children;
	
public:
	bool isVisible{ false };
	unsigned int zIndex;
	const bool followParentVisibility;
	const std::function<XMFLOAT2(const float width, const float height)> calculatePosition;

	UIComponent(UIComponentArgs uiComponentArgs, const bool followParentVisibility = true);
	void Translate(XMFLOAT2 vector) { localPosition = localPosition + vector; }
	std::vector<UIComponent*>& GetChildren() { return children; }
	XMFLOAT2 GetWorldPosition() const;
	UIComponent* GetParent() const { return parent; }
	void SetParent(UIComponent& parent) { this->parent = &parent; }
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
	void SetLocalPosition(const XMFLOAT2 pos) { this->localPosition = pos; }
	virtual const std::string GetUIAbilityDragBehavior() const { return "NONE"; }
	virtual const std::string GetUIItemDragBehavior() const { return "NONE"; }
	virtual const std::string GetUIItemRightClickBehavior() const { return "NONE"; }
	virtual void Draw() = 0;
	~UIComponent();
};