#pragma once

#include <Components/Component.h>
#include "GameObject.h"

class AIComponent : public Component
{
public:
	int targetId{ -1 };
	float swingTimer{ 0.0f };
};