#pragma once

#include <vector>
#include <DirectXMath.h>
#include "UIComponent.h"
#include "../DeviceResources.h"

struct UIComponentArgs
{
	UIComponentArgs(
		DX::DeviceResources* deviceResources,
		std::vector<UIComponent*>& uiComponents,
		const DirectX::XMFLOAT2 position,
		const Layer uiLayer,
		const unsigned int zIndex)
		: deviceResources{ deviceResources },
		  uiComponents{ uiComponents },
		  position{ position },
		  uiLayer{ uiLayer },
		  zIndex{ zIndex }
	{
	}

	DX::DeviceResources* deviceResources;
	std::vector<UIComponent*>& uiComponents;
	const DirectX::XMFLOAT2 position;
	const Layer uiLayer;
	const unsigned int zIndex;
};