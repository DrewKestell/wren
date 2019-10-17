#pragma once

#include <vector>
#include <DirectXMath.h>
#include <Layer.h>

class UIComponent;

struct UIComponentArgs
{
	UIComponentArgs(
		DX::DeviceResources* deviceResources,
		std::vector<UIComponent*>& uiComponents,
		const std::function<XMFLOAT2(const float width, const float height)> calculatePosition,
		const Layer uiLayer,
		const unsigned int zIndex)
		: deviceResources{ deviceResources },
		  uiComponents{ uiComponents },
		  calculatePosition{ calculatePosition },
		  uiLayer{ uiLayer },
		  zIndex{ zIndex }
	{
	}

	DX::DeviceResources* deviceResources;
	std::vector<UIComponent*>& uiComponents;
	const std::function<XMFLOAT2(const float width, const float height)> calculatePosition;
	const Layer uiLayer;
	const unsigned int zIndex;
};