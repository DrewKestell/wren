#pragma once

#include "UIComponent.h"
#include "UISkillListing.h"
#include "EventHandling/Observer.h"
#include "EventHandling/Events/Event.h"
#include <Models/Skill.h>

class UISkillsContainer : public UIComponent
{
	bool initialized{ false };
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Factory2* d2dFactory;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dDeviceContext;
	IDWriteFactory2* writeFactory;
	ID2D1SolidColorBrush* brush;
	IDWriteTextFormat* textFormat;
	std::unique_ptr<UISkillListing>* skillListings;
public:
	UISkillsContainer(
		std::vector<UIComponent*>& uiComponents,
		const XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex,
		ID2D1DeviceContext1* d2dDeviceContext,
		ID2D1Factory2* d2dFactory,
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dDeviceContext,
		IDWriteFactory2* writeFactory,
		ID2D1SolidColorBrush* brush,
		IDWriteTextFormat* textFormat);
	virtual void Draw();
	virtual const bool HandleEvent(const Event* const event);
	void Initialize(std::vector<WrenCommon::Skill*>* skills);
};