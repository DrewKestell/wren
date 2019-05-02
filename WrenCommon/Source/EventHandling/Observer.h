#pragma once

#include "Events/Event.h"

class Observer
{
public:	
	virtual const bool HandleEvent(const Event* const event) = 0;
};